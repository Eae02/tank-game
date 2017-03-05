#pragma once

#include "registryreference.h"
#include "luainc.h"

namespace TankGame
{
namespace Lua
{
	class Sandbox
	{
	public:
		Sandbox(lua_State* state);
		
		inline void PushTable(lua_State* state) const
		{ m_sandboxTableRef.Load(state); }
		
	private:
		RegistryReference m_sandboxTableRef;
	};
}
}
