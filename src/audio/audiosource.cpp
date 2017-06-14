#include "audiosource.h"
#include "audiobuffer.h"
#include "../settings.h"

namespace TankGame
{
	void DestroyAudioSource(ALuint source)
	{
		alDeleteSources(1, &source);
	}
	
	static ALuint CreateAudioSource()
	{
		ALuint id;
		alGenSources(1, &id);
		return id;
	}
	
	ViewInfo AudioSource::s_viewInfo;
	
	AudioSource::AudioSource(VolumeModes volumeMode)
	    : ALResource(CreateAudioSource()), m_volumeMode(volumeMode)
	{
		SetVolume(1.0f);
		SetPitch(1.0f);
	}
	
	void AudioSource::SetPosition(glm::vec2 position)
	{
		glm::vec2 sPosition(s_viewInfo.GetWorldViewMatrix() * glm::vec3(position, 1.0f));
		
		alSource3f(GetID(), AL_POSITION, sPosition.x, sPosition.y, 0);
	}
	
	void AudioSource::SetVelocity(glm::vec2 velocity)
	{
		glm::vec2 sVelocity(s_viewInfo.GetWorldViewMatrix() * glm::vec3(velocity, 0.0f));
		
		alSource3f(GetID(), AL_VELOCITY, sVelocity.x, sVelocity.y, 0);
	}
	
	void AudioSource::SetDirection(glm::vec2 direction)
	{
		glm::vec2 sDirection(s_viewInfo.GetWorldViewMatrix() * glm::vec3(direction, 0.0f));
		
		alSource3f(GetID(), AL_DIRECTION, sDirection.x, sDirection.y, 0);
	}
	
	void AudioSource::SetAttenuationSettings(float rolloffFactor, float refDistance)
	{
		alSourcef(GetID(), AL_ROLLOFF_FACTOR, rolloffFactor);
		alSourcef(GetID(), AL_REFERENCE_DISTANCE, refDistance);
	}
	
	void AudioSource::SetVolume(float volume)
	{
		if (m_volumeMode == VolumeModes::Music)
			volume *= Settings::GetInstance().GetMusicGain();
		if (m_volumeMode == VolumeModes::Effect)
			volume *= Settings::GetInstance().GetSFXGain();
		
		alSourcef(GetID(), AL_GAIN, volume);
	}
	
	bool AudioSource::IsPlaying() const
	{
		ALint state;
		alGetSourcei(GetID(), AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}
}
