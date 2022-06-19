#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace TankGame
{
	constexpr int MIN_RES_X = 600;
	constexpr int MIN_RES_Y = 400;
	
	struct VideoModes
	{
		std::vector<glm::ivec2> resolutions;
		int defaultResolutionIndex;
	};
	
	VideoModes DetectVideoModes();
	
	double GetTime();
	
	void PlatformInitialize();
	void PlatformShutdown();
}
