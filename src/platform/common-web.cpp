#ifdef __EMSCRIPTEN__
#include "common.h"

#include <emscripten/emscripten.h>

namespace TankGame
{
	double GetTime()
	{
		return emscripten_get_now() / 1000.0;
	}
	
	VideoModes DetectVideoModes()
	{
		return { {}, -1 };
	}
	
	void PlatformInitialize()
	{
		
	}
	
	void PlatformShutdown()
	{
		
	}
}

#endif
