#pragma once


#include "utils/utils.h"

namespace TankGame
{
	struct ImageData
	{
		std::unique_ptr<uint8_t, FreeDeleter> data;
		uint32_t width;
		uint32_t height;
	};
	
	ImageData LoadImageData(const fs::path& path, int numChannels);
}
