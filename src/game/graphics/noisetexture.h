#pragma once

#include "gl/texture1d.h"

namespace TankGame
{
	class NoiseTexture : public Texture1D
	{
	public:
		NoiseTexture(int size, float min, float max);
	};
}
