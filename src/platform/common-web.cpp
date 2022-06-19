#ifdef __EMSCRIPTEN__
#include "common.h"

namespace TankGame
{
	double GetTime()
	{
		return //TODO;
	}
	
	VideoModes DetectVideoModes()
	{
		return { {}, -1 };
	}
}

#endif
