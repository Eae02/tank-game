#include "screenshotserializer.h"
#include "../gameworld.h"

#include <stb_image_write.h>
#include <stb_image.h>

namespace TankGame
{
	ScreenShotSerializer::ScreenShotSerializer(const GameWorld& gameWorld, int width, int height)
	    : m_aspectRatio(width / static_cast<float>(height)), m_imageWriteBuffer(width * height * 4),
	      m_outputTexture(width, height, 1, GL_RGBA8)
	{
		glNamedFramebufferTexture(m_framebuffer.GetID(), GL_COLOR_ATTACHMENT0, m_outputTexture.GetID(), 0);
		glNamedFramebufferDrawBuffer(m_framebuffer.GetID(), GL_COLOR_ATTACHMENT0);
		
		m_worldRenderer.SetWorld(&gameWorld);
		
		m_deferredRenderer.OnResize(width, height);
		m_shadowRenderer.OnResize(width, height);
	}
	
	void ScreenShotSerializer::WriteScreenShot(glm::vec2 position)
	{
		ViewInfo viewInfo(position, 0, 20, m_aspectRatio);
		
		m_worldRenderer.DrawShadowMaps(m_shadowRenderer, viewInfo);
		
		Framebuffer::Save();
		Framebuffer::Bind(m_framebuffer, 0, 0, m_outputTexture.GetWidth(), m_outputTexture.GetHeight());
		
		m_worldRenderer.Prepare(viewInfo, 0);
		m_deferredRenderer.Draw(m_worldRenderer, viewInfo);
		
		Framebuffer::Restore();
		
		glGetTextureImage(m_outputTexture.GetID(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
		                  m_imageWriteBuffer.size(), m_imageWriteBuffer.data());
		
		//Flips the image horizontally
		for (int y = 0; y < m_outputTexture.GetHeight() / 2; y++)
		{
			for (int x = 0; x < m_outputTexture.GetWidth(); x++)
			{
				int baseCoord1 = (x + y * m_outputTexture.GetWidth()) * 4;
				int baseCoord2 = (x + (m_outputTexture.GetHeight() - y - 1) * m_outputTexture.GetWidth()) * 4;
				
				for (int i = 0; i < 4; i++)
					std::swap(m_imageWriteBuffer[baseCoord1 + i], m_imageWriteBuffer[baseCoord2 + i]);
			}
		}
		
		std::ostringstream stream;
		
		if (stbi_write_png_to_func([] (void* stream, void* data, int size)
		{
			reinterpret_cast<std::ostringstream*>(stream)->write(reinterpret_cast<char*>(data), size);
		}, &stream, m_outputTexture.GetWidth(), m_outputTexture.GetHeight(), 4, m_imageWriteBuffer.data(), 0) == 0)
		{
			throw std::runtime_error("Error writing screenshot.");
		}
		
		m_imageDataStrings.push_back(stream.str());
	}
	
	void ScreenShotSerializer::WriteResult(std::ostream& outputStream) const
	{
		uint64_t imageCount = m_imageDataStrings.size();
		outputStream.write(reinterpret_cast<const char*>(&imageCount), sizeof(imageCount));
		
		for (uint32_t i = 0; i < m_imageDataStrings.size(); i++)
		{
			uint64_t imageBytes = m_imageDataStrings[i].size();
			outputStream.write(reinterpret_cast<const char*>(&imageBytes), sizeof(imageBytes));
			
			outputStream.write(m_imageDataStrings[i].data(), imageBytes);
		}
	}
	
	std::vector<Texture2D> ScreenShotSerializer::Deserialize(std::istream& stream)
	{
		uint64_t imageCount;
		stream.read(reinterpret_cast<char*>(&imageCount), sizeof(uint64_t));
		
		std::vector<Texture2D> images;
		images.reserve(imageCount);
		
		std::vector<stbi_uc> readBuffer;
		
		for (uint32_t i = 0; i < imageCount; i++)
		{
			uint64_t imageBytes;
			stream.read(reinterpret_cast<char*>(&imageBytes), sizeof(uint64_t));
			
			readBuffer.resize(imageBytes);
			stream.read(reinterpret_cast<char*>(readBuffer.data()), imageBytes);
			
			int width, height;
			std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> imageData(
				stbi_load_from_memory(readBuffer.data(), imageBytes, &width, &height, nullptr, 4), &stbi_image_free
			);
			
			if (imageData == nullptr)
				throw std::runtime_error("Invalid screen shot data.");
			
			Texture2D texture(width, height, 1, GL_RGBA8);
			
			glTextureSubImage2D(texture.GetID(), 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, imageData.get());
			
			texture.SetWrapMode(GL_CLAMP_TO_EDGE);
			texture.SetupMipmapping(true);
			
			glTextureParameteri(texture.GetID(), GL_TEXTURE_SWIZZLE_A, GL_ONE);
			
			images.emplace_back(std::move(texture));
		}
		
		return images;
	}
}
