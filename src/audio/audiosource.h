#pragma once

#include "alresource.h"
#include "audiobuffer.h"
#include "../graphics/viewinfo.h"

namespace TankGame
{
	class AudioSource : public ALResource
	{
	public:
		enum class VolumeModes
		{
			Music,
			Effect,
			None
		};
		
		explicit AudioSource(VolumeModes volumeMode = VolumeModes::None);
		
		void SetBuffer(const AudioBuffer& buffer);
		
		void SetPosition(glm::vec2 position);
		void SetVelocity(glm::vec2 velocity);
		void SetDirection(glm::vec2 direction);
		
		void SetIsLooping(bool isLooping);
		
		void SetAttenuationSettings(float rolloffFactor, float refDistance);
		
		void Play() const;
		void Stop() const;
		
		void SetVolume(float volume);
		void SetPitch(float pitch);
		
		bool IsPlaying() const;
		bool IsStopped() const;
		
	private:
		VolumeModes m_volumeMode;
		
		float m_currentVolume = 0;
		float m_currentPitch = 0;
		glm::vec2 m_currentPosition;
		glm::vec2 m_currentVelocity;
		glm::vec2 m_currentDirection;
	};
}
