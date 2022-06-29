#include "texture2darray.h"
#include "../../utils/utils.h"
#include "../../loadimage.h"

#include <stb_image_resize.h>

namespace TankGame
{
	Texture2DArray::Texture2DArray(GLsizei width, GLsizei height, GLsizei layers, GLsizei levels, TextureFormat format)
	    : Texture(levels, format), m_width(width), m_height(height), m_layers(layers)
	{
		GLuint texture;
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture);
		SetID(texture);
		
		if (hasTextureStorage)
		{
			glTextureStorage3D(texture, levels, TextureFormatGL::InternalFormat[(int)format], width, height, layers);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
			glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, TextureFormatGL::InternalFormat[(int)format], width, height, layers, 0,
			             TextureFormatGL::Format[(int)format], TextureFormatGL::Type[(int)format], nullptr);
		}
		
		glTextureParameteri(texture, GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(texture, GL_TEXTURE_MAX_LEVEL, levels - 1);
		
		SetMinFilter(GL_LINEAR);
		SetMagFilter(GL_LINEAR);
		SetWrapR(GL_CLAMP_TO_EDGE);
	}
	
	void Texture2DArray::LoadLayerFromFile(int layer, const fs::path& path)
	{
		ImageData imageData = LoadImageData(path, GetNumComponents());
		
		std::unique_ptr<uint8_t, FreeDeleter> data;
		
		if (imageData.width != (uint32_t)m_width || imageData.height != (uint32_t)m_height)
		{
#if defined(NDEBUG) || defined(__EMSCRIPTEN__)
			Panic("Cannot resize image for tile grid material: '" + path.string() + "'.");
#else
			GetLogStream() << LOG_WARNING << "Resizing image '" << path.string() << "' from "
				<< imageData.width << "x" << imageData.height << " to " << m_width << "x" << m_height << "\n";
			
			data.reset(static_cast<uint8_t*>(std::malloc(m_width * m_height * GetNumComponents())));
			stbir_resize_uint8(
				imageData.data.get(), imageData.width, imageData.height, 0,
				data.get(), m_width, m_height, 0, GetNumComponents());
#endif
		}
		else
		{
			data = std::move(imageData.data);
		}
		
		glTextureSubImage3D(
			GetID(), 0, 0, 0, layer, m_width, m_height, 1, TextureFormatGL::Format[(int)GetFormat()], GL_UNSIGNED_BYTE, data.get());
	}
}
