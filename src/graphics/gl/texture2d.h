#pragma once

#include <string>
#include <span>

#include "texture.h"
#include "../../utils/filesystem.h"

namespace TankGame
{
	class Texture2D : public Texture
	{
	public:
		static Texture2D FromFile(const fs::path& path, int numChannels);
		
		Texture2D(GLsizei width, GLsizei height, GLsizei levels, TextureFormat internalFormat);
		
		void SetWrapMode(int wrapMode);
		
		inline void SetWrapS(int wrapS)
		{ glTextureParameteri(GetID(), GL_TEXTURE_WRAP_S, wrapS); }
		inline void SetWrapT(int wrapT)
		{ glTextureParameteri(GetID(), GL_TEXTURE_WRAP_T, wrapT); }
		
		inline GLsizei GetWidth() const
		{ return m_width; }
		inline GLsizei GetHeight() const
		{ return m_height; }
		
		void SetData(std::span<const char> data)
		{
			SetSubData(0, 0, m_width, m_height, data);
		}
		
		void SetSubData(uint32_t x, uint32_t y, uint32_t width, uint32_t height, std::span<const char> data);
		
	private:
		GLsizei m_width;
		GLsizei m_height;
	};
}
