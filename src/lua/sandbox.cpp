#include "sandbox.h"

namespace TankGame
{
namespace Lua
{
	Lua::Sandbox::Sandbox(lua_State* state)
	{
		lua_newtable(state); //Creates the sandbox table
		
		lua_newtable(state); //Creates the sandbox metatable
		
		lua_getglobal(state, "_G");
		lua_setfield(state, -2, "__index");
		lua_setmetatable(state, -2);
		
		m_sandboxTableRef = RegistryReference::PopAndCreate(state);
	}
}
}
