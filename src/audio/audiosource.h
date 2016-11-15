#pragma once

#include "alresource.h"
#include "../graphics/viewinfo.h"

#include <glm/glm.hpp>

namespace TankGame
{
	void DestroyAudioSource(ALuint source);
	
	class AudioSource : public ALResource<&DestroyAudioSource>
	{
	public:
		AudioSource();
		
		void SetBuffer(const class AudioBuffer& buffer);
		
		void SetPosition(glm::vec2 position);
		void SetVelocity(glm::vec2 velocity);
		void SetDirection(glm::vec2 direction);
		void SetIsLooping(bool isLooping);
		
		void SetAttenuationSettings(float rolloffFactor, float refDistance);
		
		void Play(float volume, float pitch) const;
		void Stop() const;
		
		bool IsPlaying() const;
		
		inline static void SetViewInfo(const ViewInfo& viewInfo)
		{ s_viewInfo = viewInfo; }
		
	private:
		static ViewInfo s_viewInfo;
		
		mutable float m_volume = 1;
		mutable float m_pitch = 1;
	};
}
