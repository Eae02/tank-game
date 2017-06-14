#pragma once

#include "luainc.h"

#include <memory>
#include <string>
#include <functional>

namespace TankGame
{
namespace Lua
{
	class Sandbox;
	
	void Init();
	void Destroy();
	
	void CallFunction(int numParams, int numReturnValues);
	void DoString(const std::string& code, const Sandbox* sandbox);
	
	void PushFunction(lua_State* state, std::function<int(lua_State*)> callback);
	
	lua_State* GetState();
}
}
