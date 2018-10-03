#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "audiosource.h"

namespace TankGame
{
	class SoundEffectPlayer
	{
	public:
		inline explicit SoundEffectPlayer(const std::string& effectName)
		    : m_effectName(effectName) { }
		
		void Play(glm::vec2 position, float volume, float pitch, float rolloffFactor = 1.0f, float refDistance = 0.0f);
		
	private:
		std::string m_effectName;
		
		std::vector<AudioSource> m_audioSources;
	};
}
