#ifdef __EMSCRIPTEN__
#include "paths.h"

namespace TankGame
{
	fs::path GetResPath()
	{
		return "/res";
	}
	
	fs::path GetDataPath()
	{
		return "/data";
	}
}

#endif
