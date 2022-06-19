#ifndef __EMSCRIPTEN__
#include "functions.h"
#include "dsawrapper.h"

#include <GLFW/glfw3.h>

namespace TankGame
{
#define GL_FUNC(name, proc) proc name;
#include "glfunctionslist.inl"
#include "glfunctionslist_dsa.inl"
#undef GL_FUNC
	
	bool hasBufferStorage;
	bool hasTextureStorage;
	
	void LoadOpenGLFunctions(bool useDSAWrapper)
	{
#define GL_FUNC(name, proc) name = reinterpret_cast<proc>(glfwGetProcAddress(#name));
#include "glfunctionslist.inl"
#include "glfunctionslist_dsa.inl"
#undef GL_FUNC
		
		if (useDSAWrapper || !glfwExtensionSupported("GL_ARB_direct_state_access"))
			DSAWrapper::InstallDSAFunctions();
		
		hasBufferStorage = glfwExtensionSupported("GL_ARB_buffer_storage") && glBufferStorage;
		hasTextureStorage = glfwExtensionSupported("GL_ARB_texture_storage") &&
			glTexStorage1D && glTexStorage2D && glTexStorage3D;
	}
}
#endif
