#ifndef __EMSCRIPTEN__
#include "common.h"
#include "../utils/utils.h"

#include <GLFW/glfw3.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif

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
			Panic("Error initializing GLFW.");
		}
		
		glfwSetErrorCallback([] (int error, const char* description)
		{
			Panic(std::to_string(error) + " " + description);
		});
	}
	
	void PlatformShutdown() { }
	
	void SyncFileSystem() { }
	
	void* DLOpen(const char* name)
	{
#ifdef _WIN32
		return reinterpret_cast<void*>(LoadLibrary(name));
#else
		return dlopen(name, RTLD_LAZY);
#endif
	}
	
	void* DLSym(void* library, const char* name)
	{
#ifdef _WIN32
		return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(library), name));
#else
		return dlsym(library, name);
#endif
	}
}

#endif
