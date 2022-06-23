#include "textureloadoperation.h"
#include "../asyncworklist.h"

namespace TankGame
{
	std::future<TextureLoadOperation> TextureLoadOperation::Start(fs::path _path, int _numChannels)
	{
		return std::async(LOADING_LAUNCH_POLICY, [path=std::move(_path), numChannels=_numChannels]
		{
			return TextureLoadOperation::Load(path, numChannels);
		});
	}
	
	TextureLoadOperation TextureLoadOperation::Load(fs::path path, int numChannels)
	{
		TextureLoadOperation op;
		op.m_imageData = LoadImageData(path, numChannels);
		op.m_numMipmaps = Texture2D::GetMipmapCount(std::max(op.m_imageData.width, op.m_imageData.height));
		op.m_numChannels = numChannels;
		op.m_path = std::move(path);
		return op;
	}
	
	Texture2D TextureLoadOperation::CreateTexture() const
	{
		TextureFormat format = TextureFormat::RGBA8;
		if (m_numChannels != 4)
			format = static_cast<TextureFormat>((int)TextureFormat::R8 + m_numChannels - 1);
		
		Texture2D texture(m_imageData.width, m_imageData.height, m_numMipmaps, format);
		
		texture.SetData({
			reinterpret_cast<char*>(m_imageData.data.get()),
			m_imageData.width * m_imageData.height * (size_t)m_numChannels
		});
		
		texture.SetupMipmapping(true);
		
		texture.SetWrapMode(GL_CLAMP_TO_EDGE);
		
		return texture;
	}
}
