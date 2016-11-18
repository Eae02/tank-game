#include "ambienceplayer.h"
#include "../utils/utils.h"
#include "../settings.h"

namespace TankGame
{
	AmbiencePlayer::AmbiencePlayer(const fs::path& dirPath)
	{
		for (const fs::directory_entry& entry : fs::directory_iterator(dirPath))
		{
			if (entry.path().extension() == ".ogg")
				m_tracks.emplace_back(AudioBuffer::FromOGG(entry.path().string()));
		}
		
		m_trackDistribution = std::uniform_int_distribution<size_t>(0, m_tracks.size());
	}
	
	void AmbiencePlayer::Stop()
	{
		if (!m_isPlaying)
			return;
		m_isPlaying = false;
		m_source.Stop();
	}
	
	void AmbiencePlayer::Update()
	{
		if (!m_isPlaying)
			return;
		
		float musicVol = Settings::GetInstance().GetMusicGain();
		if (!FloatEqual(m_volume, musicVol))
		{
			alSourcef(m_source.GetID(), AL_GAIN, musicVol);
			m_volume = musicVol;
		}
		
		if (!m_source.IsPlaying())
		{
			m_currentTrack = m_trackDistribution(randomGen);
			
			m_source.SetBuffer(m_tracks[m_currentTrack]);
			m_source.Play(m_volume, 1);
		}
	}
	
	void AmbiencePlayer::SetVolume(float volume)
	{
		m_volume = volume;
		alSourcef(m_source.GetID(), AL_GAIN, volume);
	}
}
