#include "noisetexture.h"
#include "../utils/random.h"

#include <vector>
#include <random>

namespace TankGame
{
	NoiseTexture::NoiseTexture(int size, float min, float max)
	    : Texture2D(size, 1, 1, GL_R32F)
	{
		std::uniform_real_distribution<float> noiseDist(min, max);
		
		std::vector<float> noise(size);
		for (int i = 0; i < size; i++)
			noise[i] = noiseDist(globalRNG);
		
		glTextureSubImage2D(GetID(), 0, 0, 0, size, 1, GL_RED, GL_FLOAT, noise.data());
		
		SetupMipmapping(false);
		SetMagFilter(GL_LINEAR);
		SetMinFilter(GL_LINEAR);
	}
}
