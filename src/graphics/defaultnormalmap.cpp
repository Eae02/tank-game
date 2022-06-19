#include "defaultnormalmap.h"
#include "gl/texture2d.h"
#include "../utils/utils.h"

namespace TankGame
{
	static std::unique_ptr<Texture2D> defaultNormalMap;
	
	const Texture2D& GetDefaultNormalMap()
	{
		if (defaultNormalMap == nullptr)
		{
			defaultNormalMap = std::make_unique<Texture2D>(1, 1, 1, GL_RGBA8);
			
			float pixel[] = { 0.5f, 0.5f, 1.0f, 0.0f };
			glTextureSubImage2D(defaultNormalMap->GetID(), 0, 0, 0, 1, 1, GL_RGBA, GL_FLOAT, pixel);
			
			CallOnClose([] { defaultNormalMap = nullptr; });
		}
		
		return *defaultNormalMap;
	}
}
