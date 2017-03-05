#include "activatorentity.h"
#include "../gameworld.h"
#include "../../audio/soundeffectplayer.h"
#include "../../lua/luavm.h"
#include "../../utils/imgui.h"

#include <array>
#include <imgui.h>
#include <cstring>

namespace TankGame
{
	Lua::RegistryReference ActivatorEntity::s_metaTableRef;
	
	ActivatorEntity::ActivatorEntity(std::string onActivateScript, bool singleUse)
	    : PropEntity("Activator"), m_onActivateScript(std::move(onActivateScript)), m_singleUse(singleUse)
	{
		SetZ(0.6f);
		SetSizeX(0.5f);
	}
	
	bool ActivatorEntity::CanInteract()
	{
		return !(m_singleUse && m_hasActivated);
	}
	
	void ActivatorEntity::OnInteract()
	{
		m_hasActivated = true;
		
		PushLuaInstance(Lua::GetState());
		lua_getfield(Lua::GetState(), -1, "onActivated");
		if (lua_isfunction(Lua::GetState(), -1))
			Lua::CallFunction(0, 0);
		lua_pop(Lua::GetState(), 1);
		
		if (!m_onActivateScript.empty())
		{
			if (const Lua::Sandbox* sandbox = GetGameWorld()->GetLuaSandbox())
				Lua::DoString(m_onActivateScript, sandbox);
		}
	}
	
	const char* ActivatorEntity::GetObjectName() const
	{ return "Activator"; }
	
	const char* ActivatorEntity::GetSerializeClassName() const
	{ return "Activator"; }
	
	void ActivatorEntity::RenderProperties()
	{
		RenderBaseProperties(Transform::Properties::Position);
		
		ImGui::Checkbox("Single Use", &m_singleUse);
		
		RenderScriptEditor("Activate Script", m_onActivateScript);
	}
	
	nlohmann::json ActivatorEntity::Serialize() const
	{
		nlohmann::json json;
		
		json["transform"] = GetTransform().Serialize(Transform::Properties::Position);
		json["single_use"] = m_singleUse;
		if (!m_onActivateScript.empty())
			json["activate_script"] = m_onActivateScript;
		
		return json;
	}
	
	std::unique_ptr<Entity> ActivatorEntity::Clone() const
	{
		return std::make_unique<ActivatorEntity>(*this);
	}
	
	void ActivatorEntity::PushLuaMetaTable(lua_State* state) const
	{
		if (!s_metaTableRef)
		{
			NewLuaMetaTable(state);
			
			PropEntity::PushLuaMetaTable(state);
			lua_setmetatable(state, -2);
			
			// ** hasActivated **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				lua_pushboolean(state, dynamic_cast<ActivatorEntity*>(LuaGetInstance(state))->m_hasActivated);
				return 1;
			});
			lua_setfield(state, -2, "hasActivated");
			
			s_metaTableRef = Lua::RegistryReference::PopAndCreate(state);
			CallOnClose([] { s_metaTableRef = { }; });
		}
		
		s_metaTableRef.Load(state);
	}
}
