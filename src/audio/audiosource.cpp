#include "audiosource.h"
#include "audiobuffer.h"
#include "../settings.h"

namespace TankGame
{
	void DestroyAudioSource(ALuint source)
	{
		alDeleteSources(1, &source);
	}
	
	ViewInfo AudioSource::s_viewInfo;
	
	AudioSource::AudioSource(VolumeModes volumeMode)
	    : m_volumeMode(volumeMode)
	{
		ALuint id;
		alGenSources(1, &id);
		SetID(id);
		
		alSourcef(GetID(), AL_GAIN, m_volume);
		alSourcef(GetID(), AL_PITCH, m_pitch);
	}
	
	void AudioSource::SetBuffer(const AudioBuffer& buffer)
	{
		alSourcei(GetID(), AL_BUFFER, buffer.GetID());
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
	
	void AudioSource::SetIsLooping(bool isLooping)
	{
		alSourcei(GetID(), AL_LOOPING, isLooping ? AL_TRUE : AL_FALSE);
	}
	
	void AudioSource::SetAttenuationSettings(float rolloffFactor, float refDistance)
	{
		alSourcef(GetID(), AL_ROLLOFF_FACTOR, rolloffFactor);
		alSourcef(GetID(), AL_REFERENCE_DISTANCE, refDistance);
	}
	
	void AudioSource::Play(float volume, float pitch) const
	{
		if (m_volumeMode == VolumeModes::Music)
			volume *= Settings::GetInstance().GetMusicGain();
		if (m_volumeMode == VolumeModes::Effect)
			volume *= Settings::GetInstance().GetSFXGain();
		
		if (!FloatEqual(m_volume, volume))
		{
			m_volume = volume;
			alSourcef(GetID(), AL_GAIN, volume);
		}
		
		if (!FloatEqual(m_pitch, pitch))
		{
			m_pitch = pitch;
			alSourcef(GetID(), AL_PITCH, pitch);
		}
		
		alSourcePlay(GetID());
	}
	
	void AudioSource::Stop() const
	{
		alSourceStop(GetID());
	}
	
	bool AudioSource::IsPlaying() const
	{
		ALint state;
		alGetSourcei(GetID(), AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}
}
