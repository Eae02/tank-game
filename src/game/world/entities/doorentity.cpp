#include "doorentity.h"
#include "../gameworld.h"
#include "../../audio/soundeffectplayer.h"

#include <imgui.h>
#include <algorithm>

namespace TankGame
{
	Lua::RegistryReference DoorEntity::s_metaTableRef;
	
	static SoundEffectPlayer soundEffectPlayer{ "DoorOpen" };
	
	DoorEntity::DoorEntity()
	    : PropEntity("Door", 1.0f, true) { }
	
	void DoorEntity::Open()
	{
		soundEffectPlayer.Play(GetTransform().GetPosition(), 1.0f, 1.0f, 0.5f);
		Despawn();
	}
	
	const char* DoorEntity::GetObjectName() const
	{ return "Door"; }
	const char* DoorEntity::GetSerializeClassName() const
	{ return "Door"; }
	
	void DoorEntity::RenderProperties()
	{
		RenderBaseProperties(Transform::Properties::Position | Transform::Properties::Rotation);
		
		float size = GetSizeX();
		if (ImGui::InputFloat("Size", &size))
			SetSizeX(size);
	}
	
	nlohmann::json DoorEntity::Serialize() const
	{
		nlohmann::json json;
		
		json["transform"] = GetTransform().Serialize(Transform::Properties::Position | Transform::Properties::Rotation);
		json["size"] = GetSizeX() * 2;
		
		return json;
	}
	
	std::unique_ptr<Entity> DoorEntity::Clone() const
	{
		return std::make_unique<DoorEntity>(*this);
	}
	
	void DoorEntity::PushLuaMetaTable(lua_State* state) const
	{
		if (!s_metaTableRef)
		{
			NewLuaMetaTable(state);
			
			PropEntity::PushLuaMetaTable(state);
			lua_setmetatable(state, -2);
			
			// ** open **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				dynamic_cast<DoorEntity*>(LuaGetInstance(state))->Open();
				return 0;
			});
			lua_setfield(state, -2, "open");
			
			s_metaTableRef = Lua::RegistryReference::PopAndCreate(state);
			CallOnClose([] { s_metaTableRef = { }; });
		}
		
		s_metaTableRef.Load(state);
	}
}
