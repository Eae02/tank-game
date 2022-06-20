#include "textureloadoperation.h"
#include <stb_image.h>

namespace TankGame
{
	std::future<TextureLoadOperation> TextureLoadOperation::Start(std::string path)
	{
		return std::async(std::launch::async, [p=std::move(path)]
		{
			return TextureLoadOperation::Load(p);
		});
	}
	
	TextureLoadOperation TextureLoadOperation::Load(std::string path)
	{
		TextureLoadOperation op;
		op.m_data.reset(stbi_load(path.c_str(), &op.m_width, &op.m_height, &op.m_numComponents, 0));
		if (op.m_data == nullptr)
			throw std::runtime_error("Error loading image from '" + path + "': " + stbi_failure_reason() + ".");
		op.m_numMipmaps = Texture2D::GetMipmapCount(std::max(op.m_width, op.m_height));
		op.m_path = std::move(path);
		return op;
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
}
