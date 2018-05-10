#include "registryreference.h"
#include "luavm.h"

namespace TankGame
{
namespace Lua
{
	RegistryReference::RegistryReference(RegistryReference&& other)
	    : m_index(other.m_index), m_valid(other.m_valid)
	{
		other.m_valid = false;
	}
	
	RegistryReference::RegistryReference(const RegistryReference& other)
	    : m_valid(other.m_valid), m_state(other.m_state)
	{
		if (m_valid)
		{
			other.Load(m_state);
			m_index = luaL_ref(m_state, LUA_REGISTRYINDEX);
		}
	}
	
	RegistryReference& RegistryReference::operator=(RegistryReference other)
	{
		std::swap(other.m_index, m_index);
		std::swap(other.m_valid, m_valid);
		std::swap(other.m_state, m_state);
		return *this;
	}
	
	RegistryReference::~RegistryReference()
	{
		if (m_valid)
			luaL_unref(GetState(), LUA_REGISTRYINDEX, m_index);
	}
	
	void RegistryReference::Load(lua_State* state) const
	{
		lua_rawgeti(state, LUA_REGISTRYINDEX, m_index);
	}
}
}
