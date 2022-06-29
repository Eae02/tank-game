#pragma once


#include "audiosource.h"

namespace TankGame
{
	class SoundEffectPlayer
	{
	public:
		inline explicit SoundEffectPlayer(const std::string& effectName)
		    : m_effectName(effectName) { }
		
		void Play(glm::vec2 position, float volume, float pitch, float rolloffFactor = 1.0f, float refDistance = 1.0f);
		
	private:
		std::string m_effectName;
		
		std::vector<AudioSource> m_audioSources;
	};
}
