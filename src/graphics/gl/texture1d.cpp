#include "texture1d.h"

namespace TankGame
{
	Texture1D::Texture1D(GLsizei width, GLsizei levels, GLenum internalFormat)
	    : Texture(levels, internalFormat), m_width(width)
	{
		GLuint texture;
		glCreateTextures(GL_TEXTURE_1D, 1, &texture);
		SetID(texture);
		
		glTextureStorage1D(texture, levels, internalFormat, width);
		
		SetMinFilter(GL_LINEAR);
		SetMagFilter(GL_LINEAR);
	}
}
