#include "standardfunctions.h"
#include "../utils/utils.h"

#include <ostream>

namespace TankGame
{
namespace Lua
{
	void InitStandardFunctions(lua_State* state)
	{
		lua_pushcfunction(state, [] (lua_State* state) -> int
		{
			for (int i = 1; i <= lua_gettop(state); i++)
			{
				GetLogStream() << lua_tostring(state, i);
			}
			GetLogStream() << "\n";
			
			return 0;
		});
		lua_setglobal(state, "print");
	}
}
}
