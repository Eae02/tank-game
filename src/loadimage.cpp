#include "loadimage.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#include <stb_image.h>
#endif

namespace TankGame
{
	ImageData LoadImageData(const fs::path& path, int expectedChannels)
	{
		int width, height, numChannels;
		std::unique_ptr<uint8_t, FreeDeleter> data;
		
#ifdef __EMSCRIPTEN__
		data.reset(reinterpret_cast<uint8_t*>(emscripten_get_preloaded_image_data(path.c_str(), &width, &height)));
		numChannels = 4;
#else
		data.reset(stbi_load(path.c_str(), &width, &height, &numChannels, 0));
#endif
		
		if (data == nullptr)
			Panic("Failed to load image from '" + path.string() + "'.");
		
		if (numChannels != expectedChannels)
		{
			size_t numPixels = (size_t)width * (size_t)height;
			std::unique_ptr<uint8_t, FreeDeleter> newData(static_cast<uint8_t*>(std::calloc(numPixels, expectedChannels)));
			
			for (int copyChannel = std::min(numChannels, expectedChannels) - 1; copyChannel >= 0; copyChannel--)
			{
				const uint8_t* dataIn = data.get() + copyChannel;
				uint8_t* dataOut = newData.get() + copyChannel;
				for (size_t i = 0; i < numPixels; i++)
				{
					*dataOut = *dataIn;
					dataIn += numChannels;
					dataOut += expectedChannels;
				}
			}
			
			if (expectedChannels == 4)
			{
				for (size_t i = 0; i < numPixels; i++)
					newData.get()[i * 4 + 3] = 255;
			}
			
			data = std::move(newData);
		}
		
		return ImageData { std::move(data), (uint32_t)width, (uint32_t)height };
	}
}
