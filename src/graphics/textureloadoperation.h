#pragma once

#include "gl/texture2d.h"
#include "../utils/utils.h"

#include <future>

namespace TankGame
{
	class TextureLoadOperation
	{
	public:
		TextureLoadOperation() = default;
		
		static std::future<TextureLoadOperation> Start(std::string path);
		
		static TextureLoadOperation Load(std::string path);
		
		Texture2D CreateTexture() const;
		
		inline const std::string& GetPath() const
		{ return m_path; }
		
	private:
		std::string m_path;
		
		std::unique_ptr<uint8_t, FreeDeleter> m_data;
		
		int m_width = 0;
		int m_height = 0;
		
		int m_numMipmaps = 1;
		int m_numComponents = 0;
	};
}
