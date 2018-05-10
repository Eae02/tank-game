#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//windows.h defines a bunch of macros, ugh...
#undef min
#undef max
#undef OpenEvent
#endif

#include <GL/gl.h>
#include <GL/glext.h>

namespace TankGame
{
#define GL_FUNC(name, proc) extern proc name;
#include "glfunctionslist.inl"
#undef GL_FUNC
	
	void LoadExtensions(bool useDSAWrapper = false);
}
