#include "sandbox.h"
#include "luavm.h"
#include "../utils/ioutils.h"
#include "../profiling.h"

namespace TankGame
{
namespace Lua
{
	static std::string standardLuaCode;
	static bool standardCodeLoaded = false;
	
	Sandbox::Sandbox(lua_State* state)
	{
		lua_newtable(state); //Creates the sandbox table
		
		lua_newtable(state); //Creates the sandbox metatable
		
		lua_getglobal(state, "_G");
		lua_setfield(state, -2, "__index");
		lua_setmetatable(state, -2);
		
		//setTimer
		PushFunction(state, [this] (lua_State* state) -> int
		{
			if (!lua_isfunction(state, 1))
				luaL_error(state, "Invalid argument #1 to setTimer, expected a function.");
			
			float timeRemaining = static_cast<float>(luaL_optnumber(state, 2, 1));
			
			lua_pushvalue(state, 1);
			m_timers.push_back({ timeRemaining, RegistryReference::PopAndCreate(state) });
			
			return 0;
		});
		lua_setfield(state, -2, "setTimer");
		
		//addUpdateCallback
		PushFunction(state, [this] (lua_State* state) -> int
		{
			if (!lua_isfunction(state, 1))
				luaL_error(state, "Invalid argument #1 to addUpdateCallback, expected a function.");
			
			const uint64_t id = m_nextUpdateCallbackID++;
			
			lua_pushvalue(state, 1);
			m_updateCallbacks.push_back({ id, RegistryReference::PopAndCreate(state) });
			
			*reinterpret_cast<uint64_t*>(lua_newuserdata(state, sizeof(id))) = id;
			
			return 1;
		});
		lua_setfield(state, -2, "addUpdateCallback");
		
		//removeUpdateCallback
		PushFunction(state, [this] (lua_State* state) -> int
		{
			const uint64_t id = *reinterpret_cast<uint64_t*>(lua_touserdata(state, 1));
			m_updateCallbacks.remove_if([&] (const UpdateCallback& callback) { return callback.m_id == id; });
			
			return 0;
		});
		lua_setfield(state, -2, "removeUpdateCallback");
		
		m_sandboxTableRef = RegistryReference::PopAndCreate(state);
		
		if (!standardCodeLoaded)
		{
			standardLuaCode = ReadFileContents(resDirectoryPath / "std.lua");
			standardCodeLoaded = true;
		}
		
		DoString(standardLuaCode, this);
	}
	
	void Sandbox::Update(float dt)
	{
		FUNC_TIMER
		
		for (auto it = m_updateCallbacks.begin(); it != m_updateCallbacks.end(); ++it)
		{
			it->m_callback.Load(GetState());
			lua_pushnumber(GetState(), static_cast<lua_Number>(dt));
			CallFunction(1, 0);
		}
		
		for (auto it = m_timers.begin(); it != m_timers.end();)
		{
			it->m_timeRemaining -= dt;
			
			if (it->m_timeRemaining < 0.0f)
			{
				it->m_callback.Load(GetState());
				CallFunction(0, 0);
				
				it = m_timers.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}
}
