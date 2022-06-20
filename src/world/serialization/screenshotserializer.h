#pragma once

#include "../../graphics/worldrenderer.h"
#include "../../graphics/deferredrenderer.h"

#include <sstream>

namespace TankGame
{
	class ScreenShotSerializer
	{
	public:
		explicit ScreenShotSerializer(const class GameWorld& gameWorld, int width, int height);
		
		void WriteScreenShot(glm::vec2 position);
		
		void WriteResult(std::ostream& outputStream) const;
		
		static std::vector<Texture2D> Deserialize(std::istream& stream);
		
	private:
		float m_aspectRatio;
		
		std::vector<char> m_imageWriteBuffer;
		
		Texture2D m_outputTexture;
		Framebuffer m_framebuffer;
		
		std::vector<std::string> m_imageDataStrings;
		
		ShadowRenderer m_shadowRenderer;
		WorldRenderer m_worldRenderer;
		DeferredRenderer m_deferredRenderer;
	};
}
