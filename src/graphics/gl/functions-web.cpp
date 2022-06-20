#ifdef __EMSCRIPTEN__
#include "functions.h"
#include "dsawrapper.h"

namespace TankGame
{
#define GL_FUNC(name, proc) proc name;
#include "glfunctionslist_dsa.inl"
#undef GL_FUNC
	
	void LoadOpenGLFunctions(bool useDSAWrapper)
	{
		DSAWrapper::InstallDSAFunctions();
	}
}
#endif
