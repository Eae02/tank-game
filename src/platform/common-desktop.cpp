#ifndef __EMSCRIPTEN__
#include "common.h"
#include "../exceptions/fatalexception.h"

#include <GLFW/glfw3.h>
#include <algorithm>

namespace TankGame
{
	double GetTime()
	{
		return glfwGetTime();
	}
	
	struct IVec2Compare
	{
		bool operator()(const glm::ivec2& a, const glm::ivec2& b) const
		{
			return std::make_pair(a.x, a.y) < std::make_pair(b.x, b.y);
		}
	};
	
	VideoModes DetectVideoModes()
	{
		VideoModes modes;
		
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		int videoModeCount;
		const GLFWvidmode* videoModes = glfwGetVideoModes(primaryMonitor, &videoModeCount);
		
		for (int i = 0; i < videoModeCount; i++)
		{
			if (videoModes[i].width >= MIN_RES_X && videoModes[i].height >= MIN_RES_Y)
			{
				modes.resolutions.emplace_back(videoModes[i].width, videoModes[i].height);
			}
		}
		
		std::sort(modes.resolutions.begin(), modes.resolutions.end(), IVec2Compare());
		modes.resolutions.erase(std::unique(modes.resolutions.begin(), modes.resolutions.end()), modes.resolutions.end());
		
		const GLFWvidmode* defaultVideoMode = glfwGetVideoMode(primaryMonitor);
		glm::ivec2 defaultResolution(defaultVideoMode->width, defaultVideoMode->height);
		auto defIt = std::lower_bound(modes.resolutions.begin(), modes.resolutions.end(), defaultResolution, IVec2Compare());
		if (defIt != modes.resolutions.end() && *defIt == defaultResolution)
			modes.defaultResolutionIndex = defIt - modes.resolutions.begin();
		else
			modes.defaultResolutionIndex = modes.resolutions.size() - 1;
		
		return modes;
	}
	
	void PlatformInitialize()
	{
		if (!glfwInit())
		{
			throw FatalException("Error initializing GLFW.");
		}
		
		glfwSetErrorCallback([] (int error, const char* description)
		{
			throw FatalException(std::to_string(error) + " " + description);
		});
	}
	
	void PlatformShutdown()
	{
		
	}
}

#endif