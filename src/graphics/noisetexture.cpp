#include "noisetexture.h"
#include "../utils/random.h"

#include <vector>
#include <random>

namespace TankGame
{
	NoiseTexture::NoiseTexture(int size, float min, float max)
	    : Texture2D(size, 1, 1, TextureFormat::R32F)
	{
		std::uniform_real_distribution<float> noiseDist(min, max);
		
		std::vector<float> noise(size);
		for (int i = 0; i < size; i++)
			noise[i] = noiseDist(globalRNG);
		
		SetData({ reinterpret_cast<const char*>(noise.data()), noise.size() * sizeof(float) });
		
		SetupMipmapping(false);
		SetMagFilter(GL_LINEAR);
		SetMinFilter(GL_LINEAR);
	}
}
