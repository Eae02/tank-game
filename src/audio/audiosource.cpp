#include "audiosource.h"
#include "audiobuffer.h"
#include "../settings.h"

#include <AL/al.h>

namespace TankGame
{
	static ALuint CreateAudioSource()
	{
		ALuint id;
		alGenSources(1, &id);
		return id;
	}
	
	AudioSource::AudioSource(VolumeModes volumeMode)
	    : ALResource(CreateAudioSource(), alDeleteSources), m_volumeMode(volumeMode)
	{
		SetVolume(1.0f);
		SetPitch(1.0f);
		alSourcei(GetID(), AL_SOURCE_RELATIVE, false);
		SetVelocity(glm::vec2(0.0f));
		SetDirection(glm::vec2(0.0f));
	}
	
	void AudioSource::SetBuffer(const AudioBuffer& buffer)
	{
		alSourcei(GetID(), AL_BUFFER, buffer.GetID());
	}
	
	void AudioSource::SetIsLooping(bool isLooping)
	{
		alSourcei(GetID(), AL_LOOPING, isLooping ? AL_TRUE : AL_FALSE);
	}
	
	void AudioSource::Play() const
	{
		alSourcePlay(GetID());
	}
	
	void AudioSource::Stop() const
	{
		alSourceStop(GetID());
	}
	
	void AudioSource::SetPitch(float pitch)
	{
		alSourcef(GetID(), AL_PITCH, pitch);
	}
	
	void AudioSource::SetPosition(glm::vec2 position)
	{
		alSource3f(GetID(), AL_POSITION, position.x, position.y, 0);
	}
	
	void AudioSource::SetVelocity(glm::vec2 velocity)
	{
		alSource3f(GetID(), AL_VELOCITY, velocity.x, velocity.y, 0);
	}
	
	void AudioSource::SetDirection(glm::vec2 direction)
	{
		alSource3f(GetID(), AL_DIRECTION, direction.x, direction.y, 0);
	}
	
	void AudioSource::SetAttenuationSettings(float rolloffFactor, float refDistance)
	{
		alSourcef(GetID(), AL_ROLLOFF_FACTOR, rolloffFactor);
		alSourcef(GetID(), AL_REFERENCE_DISTANCE, refDistance);
	}
	
	void AudioSource::SetVolume(float volume)
	{
		if (m_volumeMode == VolumeModes::Music)
			volume *= Settings::instance.GetMusicGain();
		if (m_volumeMode == VolumeModes::Effect)
			volume *= Settings::instance.GetSFXGain();
		
		alSourcef(GetID(), AL_GAIN, volume);
	}
	
	bool AudioSource::IsPlaying() const
	{
		ALint state;
		alGetSourcei(GetID(), AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}
	
	bool AudioSource::IsStopped() const
	{
		ALint state;
		alGetSourcei(GetID(), AL_SOURCE_STATE, &state);
		return state == AL_STOPPED;
	}
}
