#include "texture2darray.h"

#include <stb_image.h>
#include <stb_image_resize.h>
#include <memory>

namespace TankGame
{
	Texture2DArray::Texture2DArray(GLsizei width, GLsizei height, GLsizei layers, GLsizei levels, GLenum internalFormat)
	    : Texture(levels, internalFormat), m_width(width), m_height(height), m_layers(layers)
	{
		GLuint texture;
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture);
		SetID(texture);
		
		glTextureStorage3D(texture, levels, internalFormat, width, height, layers);
		
		SetMinFilter(GL_LINEAR);
		SetMagFilter(GL_LINEAR);
		SetWrapR(GL_CLAMP_TO_EDGE);
	}
	
	void Texture2DArray::LoadLayerFromFile(int layer, const fs::path& path)
	{
		int width, height, components;
		
		std::string pathString = path.string();
		
		std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> data(
				stbi_load(pathString.c_str(), &width, &height, &components, 0), &stbi_image_free
		);
		
		if (data == nullptr)
			throw std::runtime_error("Error reading image from '" + pathString + "'.");
		
		std::unique_ptr<stbi_uc[]> resizedData;
		
		if (width != m_width || height != m_height)
		{
			resizedData = std::make_unique<stbi_uc[]>(m_width * m_height * components);
			stbir_resize_uint8(data.get(), width, height, 0, resizedData.get(), m_width, m_height, 0, components);
		}
		
		const GLenum FORMATS[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
		
		glTextureSubImage3D(GetID(), 0, 0, 0, layer, m_width, m_height, 1, FORMATS[components - 1],
		                    GL_UNSIGNED_BYTE, resizedData == nullptr ? data.get() : resizedData.get());
	}
	
	void Texture2DArray::ClearLayer(int layer, GLenum format, GLenum type, const void* data)
	{
		glClearTexSubImage(GetID(), 0, 0, 0, layer, m_width, m_height, 1, format, type, data);
	}
}
