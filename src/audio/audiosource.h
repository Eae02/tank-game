#pragma once

#include "alresource.h"
#include "audiobuffer.h"
#include "../graphics/viewinfo.h"

#include <glm/glm.hpp>

namespace TankGame
{
	void DestroyAudioSource(ALuint source);
	
	class AudioSource : public ALResource<&DestroyAudioSource>
	{
	public:
		enum class VolumeModes
		{
			Music,
			Effect,
			None
		};
		
		explicit AudioSource(VolumeModes volumeMode = VolumeModes::None);
		
		inline void SetBuffer(const AudioBuffer& buffer)
		{
			alSourcei(GetID(), AL_BUFFER, buffer.GetID());
		}
		
		void SetPosition(glm::vec2 position);
		void SetVelocity(glm::vec2 velocity);
		void SetDirection(glm::vec2 direction);
		
		inline void SetIsLooping(bool isLooping)
		{
			alSourcei(GetID(), AL_LOOPING, isLooping ? AL_TRUE : AL_FALSE);
		}
		
		void SetAttenuationSettings(float rolloffFactor, float refDistance);
		
		inline void Play() const
		{
			alSourcePlay(GetID());
		}
		
		inline void Stop() const
		{
			alSourceStop(GetID());
		}
		
		void SetVolume(float volume);
		
		inline void SetPitch(float pitch)
		{
			alSourcef(GetID(), AL_PITCH, pitch);
		}
		
		bool IsPlaying() const;
		
		inline static void SetViewInfo(const ViewInfo& viewInfo)
		{
			s_viewInfo = viewInfo;
		}
		
	private:
		static ViewInfo s_viewInfo;
		
		VolumeModes m_volumeMode;
	};
}
