#pragma once

#include "../utils/utils.h"

#define lua_writestring(s, l) Nova::GetLogStream().write(s, l)
#define lua_writeline() Nova::GetLogStream() << "\n"

extern "C"
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}
