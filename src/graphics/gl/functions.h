#pragma once

#include <GL/gl.h>
#include <GL/glext.h>

namespace TankGame
{
#define GL_FUNC(name, proc) extern proc name;
#include "glfunctionslist.inl"
#undef GL_FUNC
	
	void LoadExtensions(bool useDSAWrapper = false);
}
