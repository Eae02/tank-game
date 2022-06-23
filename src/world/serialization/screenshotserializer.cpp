#include "screenshotserializer.h"
#include "../gameworld.h"

#include <stb_image_write.h>
#include <stb_image.h>

namespace TankGame
{
	ScreenShotSerializer::ScreenShotSerializer(const GameWorld& gameWorld, int width, int height)
	    : m_aspectRatio(width / static_cast<float>(height)), m_imageWriteBuffer(width * height * 4),
	      m_outputTexture(width, height, 1, TextureFormat::RGBA8)
	{
		glNamedFramebufferTexture(m_framebuffer.GetID(), GL_COLOR_ATTACHMENT0, m_outputTexture.GetID(), 0);
		glNamedFramebufferDrawBuffer(m_framebuffer.GetID(), GL_COLOR_ATTACHMENT0);
		
		m_worldRenderer.SetWorld(&gameWorld);
		
		m_deferredRenderer.CreateFramebuffer(width, height);
		m_shadowRenderer.OnResize(width, height);
	}
	
	void ScreenShotSerializer::WriteScreenShot(glm::vec2 position)
	{
#ifndef __EMSCRIPTEN__
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
			Panic("Error writing screenshot.");
		}
		
		m_imageDataStrings.push_back(stream.str());
#endif
	}
	
	void ScreenShotSerializer::WriteResult(std::ostream& outputStream) const
	{
#ifndef __EMSCRIPTEN__
		uint64_t imageCount = m_imageDataStrings.size();
		outputStream.write(reinterpret_cast<const char*>(&imageCount), sizeof(imageCount));
		
		for (uint32_t i = 0; i < m_imageDataStrings.size(); i++)
		{
			uint64_t imageBytes = m_imageDataStrings[i].size();
			outputStream.write(reinterpret_cast<const char*>(&imageBytes), sizeof(imageBytes));
			
			outputStream.write(m_imageDataStrings[i].data(), imageBytes);
		}
#endif
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
				Panic("Invalid screen shot data.");
			
			//Set alpha to 255
			for (int i = width * height - 1; i >= 0; i--)
				imageData.get()[i * 4 + 3] = 255;
			
			Texture2D texture(width, height, 1, TextureFormat::RGBA8);
			
			texture.SetData({ reinterpret_cast<char*>(imageData.get()), (size_t)width * (size_t)height * 4 });
			
			texture.SetWrapMode(GL_CLAMP_TO_EDGE);
			texture.SetupMipmapping(true);
			
			images.emplace_back(std::move(texture));
		}
		
		return images;
	}
}
