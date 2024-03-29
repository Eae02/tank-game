#include "soundeffectplayer.h"
#include "soundsmanager.h"

namespace TankGame
{
	void SoundEffectPlayer::Play(glm::vec2 position, float volume, float pitch, float rolloffFactor, float refDistance)
	{
		auto pos = std::find_if(m_audioSources.begin(), m_audioSources.end(), [] (const AudioSource& source)
		{
			return source.IsStopped();
		});
		
		AudioSource* audioSource;
		
		if (pos != m_audioSources.end())
		{
			audioSource = &*pos;
		}
		else
		{
			m_audioSources.emplace_back(AudioSource::VolumeModes::Effect);
			audioSource = &m_audioSources.back();
			
			audioSource->SetBuffer(SoundsManager::GetInstance().GetSound(m_effectName));
		}
		
		audioSource->SetVolume(volume);
		audioSource->SetPitch(pitch);
		audioSource->SetAttenuationSettings(rolloffFactor, refDistance);
		audioSource->SetPosition(position);
		audioSource->Play();
	}
}
