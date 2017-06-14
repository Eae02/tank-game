#include "luavm.h"
#include "sandbox.h"
#include "standardfunctions.h"
#include "../utils/utils.h"
#include "../utils/ioutils.h"

#include <ostream>

namespace TankGame
{
namespace Lua
{
	static lua_State* state = nullptr;
	static int errorFuncLocation;
	
	static int ErrorFunction(lua_State* state)
	{
		const char* errorMessage = lua_tostring(state, 1);
		lua_pop(state, 1);
		
		lua_getglobal(state, "debug");
		lua_getfield(state, -1, "traceback");
		
		lua_call(state, 0, 1);
		
		const char* stackTrace = lua_tostring(state, -1);
		
		lua_pop(state, 2);
		
		GetLogStream() << LOG_ERROR << "Lua error: " << errorMessage << "\n" << stackTrace;
		
		return 1;
	}
	
	void Init()
	{
		state = luaL_newstate();
		
		luaL_openlibs(state);
		
		lua_pushcfunction(state, ErrorFunction);
		errorFuncLocation = lua_gettop(state);
		
		InitStandardFunctions(state);
		
		luaL_newmetatable(state, "Entity");
		lua_pop(state, 1);
	}
	
	void Destroy()
	{
		lua_close(state);
		state = nullptr;
	}
	
	lua_State* GetState()
	{
		return state;
	}
	
	void CallFunction(int numParams, int numReturnValues)
	{
		int result = lua_pcall(state, numParams, numReturnValues, errorFuncLocation);
		
		if (result == LUA_ERRMEM)
			throw std::bad_alloc();
		if (result == LUA_ERRERR)
			throw std::runtime_error("Invalid result");
	}
	
	void DoString(const std::string& code, const Sandbox* sandbox)
	{
		if (luaL_loadstring(state, code.c_str()) != 0)
		{
			GetLogStream() << LOG_ERROR << lua_tostring(state, -1) << "\n";
			return;
		}
		
		if (sandbox != nullptr)
		{
			sandbox->PushTable(state);
			lua_setupvalue(state, -2, 1);
		}
		
		CallFunction(0, 0);
	}
	
	void PushFunction(lua_State* state, std::function<int (lua_State*)> callback)
	{
		new (lua_newuserdata(state, sizeof(callback))) decltype(callback) (std::move(callback));
		
		//Creates a metatable for the function userdata
		lua_newtable(state);
		
		//Assigns a finalizer which explicitly invokes the std::function destructor
		lua_pushcfunction(state, [] (lua_State* state) -> int
		{
			reinterpret_cast<decltype(callback)*>(lua_touserdata(state, 1))->~function();
			return 0;
		});
		lua_setfield(state, -2, "__gc");
		
		lua_setmetatable(state, -2);
		
		lua_pushcclosure(state, [] (lua_State* state) -> int
		{
			return (*reinterpret_cast<decltype(callback)*>(lua_touserdata(state, lua_upvalueindex(1))))(state);
		}, 1);
	}
}
}
