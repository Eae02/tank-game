#pragma once

#include "gl/texture2d.h"
#include "../utils/utils.h"
#include "../loadimage.h"

#include <future>

namespace TankGame
{
	class TextureLoadOperation
	{
	public:
		TextureLoadOperation() = default;
		
		static std::future<TextureLoadOperation> Start(fs::path path, int numChannels);
		
		static TextureLoadOperation Load(fs::path path, int numChannels);
		
		Texture2D CreateTexture() const;
		
		inline const fs::path& GetPath() const
		{ return m_path; }
		
	private:
		fs::path m_path;
		
		ImageData m_imageData;
		
		int m_numMipmaps = 1;
		int m_numChannels = 0;
	};
}
