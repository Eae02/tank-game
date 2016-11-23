#include "textureloadoperation.h"
#include <stb_image.h>

namespace TankGame
{
	TextureLoadOperation::TextureLoadOperation(std::string path, TextureLoadOperation::DoneCallback doneCallback)
	    : m_doneCallback(doneCallback), m_path(std::move(path)), m_data(nullptr, &STBIDataDeleter) { }
	
	TextureLoadOperation::TextureLoadOperation(const fs::path& path, TextureLoadOperation::DoneCallback doneCallback)
	    : TextureLoadOperation(path.string(), doneCallback) { }
	
	void TextureLoadOperation::DoWork()
	{
		m_data.reset(stbi_load(m_path.c_str(), &m_width, &m_height, &m_numComponents, 0));
		
		if (m_data == nullptr)
			throw std::runtime_error("Error loading image from '" + m_path + "': " + stbi_failure_reason() + ".");
		
		m_numMipmaps = Texture2D::GetMipmapCount(std::max(m_width, m_height));
	}
	
	void TextureLoadOperation::ProcessResult()
	{
		if (m_doneCallback != nullptr)
			m_doneCallback(CreateTexture());
	}
	
	Texture2D TextureLoadOperation::CreateTexture() const
	{
		const GLenum INTERNAL_FORMATS[] = { GL_R8, GL_RG8, GL_RGB8, GL_RGBA8 };
		const GLenum FORMATS[] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };
		
		Texture2D texture(m_width, m_height, m_numMipmaps, INTERNAL_FORMATS[m_numComponents - 1]);
		
		glTextureSubImage2D(texture.GetID(), 0, 0, 0, m_width, m_height, FORMATS[m_numComponents - 1],
		                    GL_UNSIGNED_BYTE, m_data.get());
		
		texture.SetupMipmapping(true);
		
		texture.SetWrapMode(GL_CLAMP_TO_EDGE);
		
		return texture;
	}
	
	void TextureLoadOperation::STBIDataDeleter(uint8_t* data)
	{
		stbi_image_free(data);
	}
}
