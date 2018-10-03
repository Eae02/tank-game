#pragma once

#include <string>

#include "texture.h"
#include "../../utils/filesystem.h"

namespace TankGame
{
	class Texture2D : public Texture
	{
	public:
		static Texture2D FromFile(const fs::path& path);
		
		Texture2D(GLsizei width, GLsizei height, GLsizei levels, GLenum internalFormat);
		
		void SetWrapMode(int wrapMode);
		
		inline void SetWrapS(int wrapS)
		{ glTextureParameteri(GetID(), GL_TEXTURE_WRAP_S, wrapS); }
		inline void SetWrapT(int wrapT)
		{ glTextureParameteri(GetID(), GL_TEXTURE_WRAP_T, wrapT); }
		
		inline GLsizei GetWidth() const
		{ return m_width; }
		inline GLsizei GetHeight() const
		{ return m_height; }
		
	private:
		GLsizei m_width;
		GLsizei m_height;
	};
}
