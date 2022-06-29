#include "texture2d.h"
#include "../../utils/utils.h"
#include "../../loadimage.h"

namespace TankGame
{
	Texture2D Texture2D::FromFile(const fs::path& path, int numChannels)
	{
		ImageData imageData = LoadImageData(path, numChannels);
		
		TextureFormat format;
		if (numChannels == 1)
			format = TextureFormat::R8;
		else if (numChannels == 2)
			format = TextureFormat::RG8;
		else if (numChannels == 4)
			format = TextureFormat::RGBA8;
		else
			Panic("Unexpected number of image channels: " + std::to_string(numChannels));
		
		Texture2D texture(imageData.width, imageData.height,
		                  GetMipmapCount(std::max(imageData.width, imageData.height)), format);
		
		texture.SetData({ reinterpret_cast<char*>(imageData.data.get()), imageData.width * imageData.height * numChannels });
		
		texture.SetupMipmapping(true);
		
		texture.SetWrapMode(GL_CLAMP_TO_EDGE);
		
		return texture;
	}
	
	Texture2D::Texture2D(GLsizei width, GLsizei height, GLsizei levels, TextureFormat format)
	    : Texture(levels, format), m_width(width), m_height(height)
	{
		GLuint texture;
		glCreateTextures(GL_TEXTURE_2D, 1, &texture);
		SetID(texture);
		
		if (hasTextureStorage)
		{
			glTextureStorage2D(texture, levels, TextureFormatGL::InternalFormat[(int)format], width, height);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, TextureFormatGL::InternalFormat[(int)format], width, height, 0,
			             TextureFormatGL::Format[(int)format], TextureFormatGL::Type[(int)format], nullptr);
		}
		
		glTextureParameteri(texture, GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(texture, GL_TEXTURE_MAX_LEVEL, levels - 1);
		SetMinFilter(GL_LINEAR);
		SetMagFilter(GL_LINEAR);
	}
	
	void Texture2D::SetSubData(uint32_t x, uint32_t y, uint32_t width, uint32_t height, std::span<const char> data)
	{
		if (data.size() != width * height * TextureFormatGL::BytesPerPixel[(int)GetFormat()])
			Panic("Incorrect texture data length");
		glTextureSubImage2D(GetID(), 0, x, y, width, height,
			TextureFormatGL::Format[(int)GetFormat()], TextureFormatGL::Type[(int)GetFormat()], data.data());
	}
	
	void Texture2D::SetWrapMode(int wrapMode)
	{
		SetWrapS(wrapMode);
		SetWrapT(wrapMode);
	}
}
