#include "texture2d.h"

#include <stb_image.h>
#include <memory>
#include <cmath>
#include <algorithm>

namespace TankGame
{
	Texture2D Texture2D::FromFile(const fs::path& path)
	{
		int width, height, components;
		
		std::string pathString = path.string();
		
		std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> data(
				stbi_load(pathString.c_str(), &width, &height, &components, 0), &stbi_image_free
		);
		
		if (data == nullptr)
			throw std::runtime_error("Error loading image from '" + pathString + "': " + stbi_failure_reason() + ".");
		
		const GLenum INTERNAL_FORMATS[] = { GL_R8, GL_RG8, GL_RGB8, GL_RGBA8 };
		const GLenum FORMATS[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
		
		int numMipmaps = GetMipmapCount(std::max(width, height));
		
		Texture2D texture(width, height, numMipmaps, INTERNAL_FORMATS[components - 1]);
		
		glTextureSubImage2D(texture.GetID(), 0, 0, 0, width, height, FORMATS[components - 1],
		                    GL_UNSIGNED_BYTE, data.get());
		
		if (components == 1)
		{
			glTextureParameteri(texture.GetID(), GL_TEXTURE_SWIZZLE_G, GL_RED);
			glTextureParameteri(texture.GetID(), GL_TEXTURE_SWIZZLE_B, GL_RED);
		}
		
		if (components < 4)
			glTextureParameteri(texture.GetID(), GL_TEXTURE_SWIZZLE_A, GL_ONE);
		
		texture.SetupMipmapping(true);
		
		texture.SetWrapMode(GL_CLAMP_TO_EDGE);
		
		return texture;
	}
	
	Texture2D::Texture2D(GLsizei width, GLsizei height, GLsizei levels, GLenum internalFormat)
	    : Texture(levels, internalFormat), m_width(width), m_height(height)
	{
		GLuint texture;
		glCreateTextures(GL_TEXTURE_2D, 1, &texture);
		SetID(texture);
		
		glTextureStorage2D(texture, levels, internalFormat, width, height);
		
		SetMinFilter(GL_LINEAR);
		SetMagFilter(GL_LINEAR);
	}
	
	void Texture2D::SetWrapMode(int wrapMode)
	{
		SetWrapS(wrapMode);
		SetWrapT(wrapMode);
	}
}
