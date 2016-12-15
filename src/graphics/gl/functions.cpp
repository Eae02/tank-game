#include "functions.h"
#include "dsawrapper.h"

#include <GLFW/glfw3.h>

namespace TankGame
{
#define GL_FUNC(name, proc) proc name;
#include "glfunctionslist.inl"
#undef GL_FUNC
	
	void LoadExtensions(bool useDSAWrapper)
	{
#define GL_FUNC(name, proc) name = reinterpret_cast<proc>(glfwGetProcAddress(#name));
#include "glfunctionslist.inl"
#undef GL_FUNC
		
		if (useDSAWrapper || !glfwExtensionSupported("GL_ARB_direct_state_access"))
			DSAWrapper::InstallDSAFunctions();
	}
}
