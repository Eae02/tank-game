#pragma once

#include "luainc.h"

namespace TankGame
{
namespace Lua
{
	class RegistryReference
	{
	public:
		inline RegistryReference() : m_valid(false) { }
		
		inline RegistryReference(int index, lua_State* state)
		    : m_index(index), m_valid(true), m_state(state) { }
		
		RegistryReference(RegistryReference&& other);
		
		RegistryReference(const RegistryReference& other);
		RegistryReference& operator=(RegistryReference other);
		
		virtual ~RegistryReference();
		
		inline static RegistryReference PopAndCreate(lua_State* state)
		{ return { luaL_ref(state, LUA_REGISTRYINDEX), state }; }
		
		inline operator bool() const
		{ return m_valid; }
		
		void Load(lua_State* state) const;
		
	private:
		int m_index;
		bool m_valid;
		lua_State* m_state;
	};
}
}
