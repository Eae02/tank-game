#pragma once

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
	
	void SyncFileSystem();
	
	void* DLOpen(const char* name);
	void* DLSym(void* library, const char* name);
	
#ifdef __EMSCRIPTEN__
	constexpr bool PLATFORM_CAN_CHANGE_VSYNC = false;
#else
	constexpr bool PLATFORM_CAN_CHANGE_VSYNC = true;
#endif
}
