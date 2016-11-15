#include "texture.h"

namespace TankGame
{
	void Texture::SetupMipmapping(bool generateMipmaps)
	{
		if (generateMipmaps)
		{
			glGenerateTextureMipmap(GetID());
			SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		}
		else
		{
			glTextureParameteri(GetID(), GL_TEXTURE_BASE_LEVEL, 0);
			glTextureParameteri(GetID(), GL_TEXTURE_MAX_LEVEL, 0);
		}
	}
	
	void DeleteTexture(GLuint id)
	{
		glDeleteTextures(1, &id);
	}
}
