#include "entity.h"
#include "gameworld.h"

#include <imgui.h>

namespace TankGame
{
	Lua::RegistryReference Entity::s_metaTableRef;
	
	Rectangle Entity::GetBoundingRectangle() const
	{
		return GetBoundingCircle().GetBoundingRectangle();
	}
	
	Circle Entity::GetBoundingCircle() const
	{
		return GetTransform().GetBoundingCircle();
	}
	
	void Entity::Despawn()
	{
		m_world->Despawn(this);
	}
	
	void Entity::OnSpawned(class GameWorld& gameWorld)
	{
		m_world = &gameWorld;
	}
	
	nlohmann::json Entity::Serialize() const
	{
		nlohmann::json json;
		json["transform"] = m_transform.Serialize(Transform::Properties::All);
		
		return json;
	}
	
	std::unique_ptr<Entity> Entity::Clone() const
	{
		return nullptr;
	}
	
	void Entity::RenderProperties()
	{
		RenderBaseProperties();
	}
	
	const char* Entity::GetObjectName() const
	{
		return "Entity";
	}
	
	void Entity::PushLuaInstance(lua_State* state)
	{
		if (!m_luaObject)
		{
			lua_newtable(state);
			
			Entity** userDataPtr = reinterpret_cast<Entity**>(lua_newuserdata(state, sizeof(Entity*)));
			*userDataPtr = this;
			
			luaL_getmetatable(state, "Entity");
			lua_setmetatable(state, -2);
			
			lua_setfield(state, -2, "__instance");
			
			PushLuaMetaTable(state);
			lua_setmetatable(state, -2);
			
			m_luaObject = Lua::RegistryReference::PopAndCreate(state);
		}
		
		m_luaObject.Load(state);
	}
	
	void Entity::RenderBaseProperties(Transform::Properties propertiesToShow, bool nameable)
	{
		if (nameable)
		{
			static std::array<char, 256> nameEditBuffer;
			
			strncpy(nameEditBuffer.data(), m_name.c_str(), nameEditBuffer.size());
			nameEditBuffer.back() = '\0';
			
			if (ImGui::InputText("Name", nameEditBuffer.data(), nameEditBuffer.size(),
			                     ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (strcmp(m_name.data(), nameEditBuffer.data()) != 0)
				{
					std::string newName(nameEditBuffer.data());
					
					if (nameEditBuffer[0] != '\0' && m_world->GetEntityByName(newName))
					{
						
					}
					else
					{
						m_name = nameEditBuffer.data();
					}
				}
			}
		}
		
		if (propertiesToShow != Transform::Properties::None &&
		    ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
		{
			GetTransform().RenderProperties(propertiesToShow);
			ImGui::TreePop();
		}
	}
	
	void Entity::PushLuaMetaTable(lua_State* state) const
	{
		if (!s_metaTableRef)
		{
			NewLuaMetaTable(state);
			
			// ** setPosition **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				LuaGetInstance(state)->GetTransform().SetPosition({ luaL_checknumber(state, 2), luaL_checknumber(state, 3) });
				return 0;
			});
			lua_setfield(state, -2, "setPosition");
			
			// ** getPosition **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				glm::vec2 position = LuaGetInstance(state)->GetTransform().GetPosition();
				lua_pushnumber(state, position.x);
				lua_pushnumber(state, position.y);
				return 2;
			});
			lua_setfield(state, -2, "getPosition");
			
			// ** setRotation **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				LuaGetInstance(state)->GetTransform().SetRotation(luaL_checknumber(state, 2));
				return 0;
			});
			lua_setfield(state, -2, "setRotation");
			
			// ** getRotation **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				lua_pushnumber(state, LuaGetInstance(state)->GetTransform().GetRotation());
				return 2;
			});
			lua_setfield(state, -2, "getRotation");
			
			// ** setScale **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				LuaGetInstance(state)->GetTransform().SetScale({ luaL_checknumber(state, 2), luaL_checknumber(state, 3) });
				return 0;
			});
			lua_setfield(state, -2, "setScale");
			
			// ** getScale **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				glm::vec2 scale = LuaGetInstance(state)->GetTransform().GetScale();
				lua_pushnumber(state, scale.x);
				lua_pushnumber(state, scale.y);
				return 2;
			});
			lua_setfield(state, -2, "getScale");
			
			// ** getObjectName **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				Entity* instance = LuaGetInstance(state);
				
				lua_pushstring(state, instance->GetObjectName());
				return 1;
			});
			lua_setfield(state, -2, "getObjectName");
			
			s_metaTableRef = Lua::RegistryReference::PopAndCreate(state);
			CallOnClose([] { s_metaTableRef = { }; });
		}
		
		s_metaTableRef.Load(state);
	}
	
	Entity* Entity::LuaGetInstance(lua_State* state)
	{
		lua_getfield(state, 1, "__instance");
		
		Entity** entity = reinterpret_cast<Entity**>(luaL_checkudata(state, -1, "Entity"));
		
		lua_pop(state, 1);
		
		return *entity;
	}
	
	void Entity::NewLuaMetaTable(lua_State* state)
	{
		lua_newtable(state);
		lua_pushvalue(state, -1);
		lua_setfield(state, -2, "__index");
	}
}
