#include "omniflamethrower.h"
#include "flamethrowerentity.h"
#include "../gameworld.h"
#include "../../updateinfo.h"

#include <glm/gtc/constants.hpp>
#include <imgui.h>

namespace TankGame
{
	Lua::RegistryReference OmniFlameThrower::s_metaTableRef;
	
	OmniFlameThrower::OmniFlameThrower(int count, float flameLength)
	    : m_count(count), m_flameLength(flameLength) { }
	
	void OmniFlameThrower::SetEnabled(bool enabled)
	{
		for (size_t i = 0; i < m_flameThrowers.size(); i++)
		{
			if (Entity* entity = m_flameThrowers[i].Get())
				static_cast<FlameThrowerEntity*>(entity)->SetEnabled(enabled);
		}
		
		m_enabled = enabled;
	}
	
	void OmniFlameThrower::Update(const UpdateInfo& updateInfo)
	{
		if (m_enabled && std::abs(GetTransform().GetRotation() - m_oldRotation) > 1E-6f)
			UpdateChildEntityTransforms();
	}
	
	void OmniFlameThrower::OnSpawned(GameWorld& gameWorld)
	{
		Entity::OnSpawned(gameWorld);
		
		if (gameWorld.GetWorldType() == GameWorld::Types::Editor)
			return;
		
		m_flameThrowers.reserve(static_cast<size_t>(m_count));
		
		//Spawns flame throwers
		for (int i = 0; i < m_count; i++)
		{
			auto entity = std::make_unique<FlameThrowerEntity>(FlameThrowerEntity::CreateManuallyControlled(m_flameLength));
			
			entity->SetEditorVisible(false);
			
			m_flameThrowers.push_back(GetGameWorld()->Spawn(std::move(entity)));
		}
		
		UpdateChildEntityTransforms();
	}
	
	void OmniFlameThrower::OnDespawning()
	{
		if (GetGameWorld()->GetWorldType() != GameWorld::Types::Editor)
		{
			for (const EntityHandle& flameThrower : m_flameThrowers)
				flameThrower.Despawn();
		}
	}
	
	void OmniFlameThrower::RenderProperties()
	{
		RenderBaseProperties(Transform::Properties::Position | Transform::Properties::Rotation);
		
		ImGui::InputInt("Flame Throwers", &m_count);
		ImGui::InputFloat("Flame Length", &m_flameLength, 0.5f, 1.0f, 1);
	}
	
	const char* OmniFlameThrower::GetObjectName() const
	{
		return "Omnidirectional Flame Thrower";
	}
	
	const char* OmniFlameThrower::GetSerializeClassName() const
	{
		return "OmniFlameThrower";
	}
	
	nlohmann::json OmniFlameThrower::Serialize() const
	{
		nlohmann::json json;
		json["transform"] = GetTransform().Serialize(Transform::Properties::Position | Transform::Properties::Rotation);
		json["flame_length"] = m_flameLength;
		json["count"] = m_count;
		return json;
	}
	
	void OmniFlameThrower::PushLuaMetaTable(lua_State* state) const
	{
		if (!s_metaTableRef)
		{
			NewLuaMetaTable(state);
			
			Entity::PushLuaMetaTable(state);
			lua_setmetatable(state, -2);
			
			// ** setEnabled **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				static_cast<OmniFlameThrower*>(LuaGetInstance(state))->SetEnabled(lua_toboolean(state, 2));
				return 0;
			});
			lua_setfield(state, -2, "setEnabled");
			
			// ** isEnabled **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				lua_pushboolean(state, static_cast<OmniFlameThrower*>(LuaGetInstance(state))->IsEnabled());
				return 1;
			});
			lua_setfield(state, -2, "isEnabled");
			
			s_metaTableRef = Lua::RegistryReference::PopAndCreate(state);
			CallOnClose([] { s_metaTableRef = { }; });
		}
		
		s_metaTableRef.Load(state);
	}
	
	void OmniFlameThrower::UpdateChildEntityTransforms()
	{
		m_oldRotation = GetTransform().GetRotation();
		
		const float rotationDelta = glm::two_pi<float>() / static_cast<float>(m_count);
		float rotation = GetTransform().GetRotation();
		
		for (size_t i = 0; i < m_flameThrowers.size(); i++)
		{
			if (Entity* entity = m_flameThrowers[i].Get())
			{
				entity->GetTransform().SetRotation(rotation);
				entity->GetTransform().SetPosition(GetTransform().GetPosition() + entity->GetTransform().GetForward());
				
				static_cast<FlameThrowerEntity*>(entity)->TransformModified();
			}
			
			rotation += rotationDelta;
		}
	}
}
