#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//windows.h defines a bunch of macros, ugh...
#undef min
#undef max
#undef OpenEvent
#endif

#ifdef __EMSCRIPTEN__
#include "glplatform-web.h"
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

namespace TankGame
{
#define GL_FUNC(name, proc) extern proc name;
#ifndef __EMSCRIPTEN__
#include "glfunctionslist.inl"
#endif
#include "glfunctionslist_dsa.inl"
#undef GL_FUNC
	
#ifdef __EMSCRIPTEN__
	constexpr bool hasBufferStorage = false;
	constexpr bool hasTextureStorage = false;
#else
	extern bool hasBufferStorage;
	extern bool hasTextureStorage;
#endif
	
	void LoadOpenGLFunctions(bool useDSAWrapper = false);
}
