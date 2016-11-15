#include "defaultnormalmap.h"
#include "gl/texture2d.h"
#include "../utils/utils.h"
#include "../utils/memory/stackobject.h"

namespace TankGame
{
	static StackObject<Texture2D> defaultNormalMap;
	
	const Texture2D& GetDefaultNormalMap()
	{
		if (defaultNormalMap.IsNull())
		{
			defaultNormalMap.Construct(2, 2, 1, GL_RGBA8);
			
			float pixel[] = { 0.5f, 0.5f, 1.0f, 0.0f };
			glClearTexImage(defaultNormalMap->GetID(), 0, GL_RGBA, GL_FLOAT, pixel);
			
			CallOnClose([] { defaultNormalMap.Destroy(); });
		}
		
		return *defaultNormalMap;
	}
}
