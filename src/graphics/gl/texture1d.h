#pragma once

#include "texture.h"

namespace TankGame
{
	class Texture1D : public Texture
	{
	public:
		Texture1D(GLsizei width, GLsizei levels, GLenum internalFormat);
		
		inline void SetWrapS(int wrapS)
		{ glTextureParameteri(GetID(), GL_TEXTURE_WRAP_S, wrapS); }
		
		inline GLsizei GetWidth() const
		{ return m_width; }
		
	private:
		GLsizei m_width;
	};
}
