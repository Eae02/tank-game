#pragma once

#include "ilightsource.h"

namespace TankGame
{
	enum class EntityShadowModes
	{
		None = 0,
		Static = 1,
		Dynamic = 2
	};
	
	class IShadowLightSource : public ILightSource
	{
	public:
		virtual class ShadowMap* GetShadowMap() const = 0;
		virtual void InvalidateShadowMap() = 0;
	};
}
