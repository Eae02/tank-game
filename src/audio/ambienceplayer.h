#pragma once

#include "audiobuffer.h"
#include "audiosource.h"

#include <random>

namespace TankGame
{
	class AmbiencePlayer
	{
	public:
		explicit AmbiencePlayer(const fs::path& dirPath);
		
		void Start()
		{ m_isPlaying = true; }
		
		void Stop();
		
		void Update();
		
	private:
		AudioSource m_source;
		
		float m_volume = 1;
		bool m_isPlaying = false;
		
		std::vector<AudioBuffer> m_tracks;
		
		size_t m_currentTrack;
		std::uniform_int_distribution<size_t> m_trackDistribution;
	};
}
