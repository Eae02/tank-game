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
			defaultNormalMap = std::make_unique<Texture2D>(1, 1, 1, TextureFormat::RGBA8);
			
			const uint8_t pixel[] = { 127, 127, 255, 255 };
			defaultNormalMap->SetData({ reinterpret_cast<const char*>(pixel), sizeof(pixel) });
			
			CallOnClose([] { defaultNormalMap = nullptr; });
		}
		
		return *defaultNormalMap;
	}
}
