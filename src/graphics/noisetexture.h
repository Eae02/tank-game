#pragma once

#include "gl/texture2d.h"

namespace TankGame
{
	class NoiseTexture : public Texture2D
	{
	public:
		NoiseTexture(int size, float min, float max);
	};
}
