#pragma once

#include "api.h"

namespace TankGame
{
	struct ClientArgs
	{
		bool profiling = false;
		bool noCursorGrab = false;
		bool useDSAWrapper = false;
	};
	
	DLL_VISIBLE void StartClient(const ClientArgs& args);
}