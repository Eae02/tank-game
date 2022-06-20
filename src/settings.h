#pragma once

#include "utils/filesystem.h"

#include <glm/glm.hpp>
#include <vector>
#include <nlohmann/json.hpp>

namespace TankGame
{
	enum class QualitySettings
	{
		Low = 0,
		Medium = 1,
		High = 2,
	};
	
	enum class ResolutionScales
	{
		_50 = 50,
		_75 = 75,
		_100 = 100,
		_125 = 125,
		_150 = 150,
		_200 = 200
	};
	
	inline bool operator >(QualitySettings a, QualitySettings b)
	{
		return static_cast<int>(a) > static_cast<int>(b);
	}
	
	inline bool operator <(QualitySettings a, QualitySettings b)
	{
		return static_cast<int>(a) < static_cast<int>(b);
	}
	
	class Settings
	{
	public:
		Settings() = default;
		
		void Load(const fs::path& jsonPath, const struct VideoModes& videoModes);
		void Save(const fs::path& jsonPath) const;
		
		inline bool IsFullscreen() const
		{ return m_isFullscreen; }
		inline void SetIsFullscreen(bool isFullscreen)
		{ m_isFullscreen = isFullscreen; }
		
		inline bool EnableVSync() const
		{ return m_enableVSync; }
		inline void SetEnableVSync(bool enableVSync)
		{ m_enableVSync = enableVSync; }
		
		inline int GetResolutionIndex() const
		{ return m_resolutionIndex; }
		inline const glm::ivec2& GetFullscreenResolution() const
		{ return m_resolution; }
		
		inline void SetResolution(int resolutionIndex, const glm::ivec2& resolution)
		{
			m_resolutionIndex = resolutionIndex;
			m_resolution = resolution;
		}
		
		void SetToDefaultVideoMode(const struct VideoModes& videoModes);
		
		inline float GetGamma() const
		{ return m_gamma; }
		inline void SetGamma(float gamma)
		{ m_gamma = gamma; }
		
		inline float GetMasterVolume() const
		{ return m_masterVolume; }
		inline float GetMusicVolume() const
		{ return m_musicVolume; }
		inline float GetSFXVolume() const
		{ return m_sfxVolume; }
		
		inline float GetMusicGain() const
		{ return m_musicVolume * m_masterVolume; }
		inline float GetSFXGain() const
		{ return m_sfxVolume * m_masterVolume; }
		
		inline void SetMasterVolume(float masterVolume)
		{ m_masterVolume = masterVolume; }
		inline void SetMusicVolume(float musicVolume)
		{ m_musicVolume = musicVolume; }
		inline void SetSFXVolume(float sfxVolume)
		{ m_sfxVolume = sfxVolume; }
		
		inline ResolutionScales GetResolutionScale() const
		{ return m_resolutionScale; }
		inline void SetResolutionScale(ResolutionScales resScale)
		{ m_resolutionScale = resScale; }
		
		inline QualitySettings GetLightingQuality() const
		{ return m_lightingQuality; }
		inline QualitySettings GetParticleQuality() const
		{ return m_particlesQuality; }
		inline QualitySettings GetPostProcessingQuality() const
		{ return m_postProcessingQuality; }
		inline bool EnableBloom() const
		{ return m_enableBloom; }
		
		inline bool QueueFrames() const
		{ return m_queueFrames; }
		inline void SetQueueFrames(bool queueFrames)
		{ m_queueFrames = queueFrames; }
		
		inline void SetLightingQuality(QualitySettings lightingQuality)
		{ m_lightingQuality = lightingQuality; }
		inline void SetParticleQuality(QualitySettings particleQuality)
		{ m_particlesQuality = particleQuality; }
		inline void SetPostProcessingQuality(QualitySettings postProcessingQuality)
		{ m_postProcessingQuality = postProcessingQuality; }
		inline void SetEnableBloom(bool enableBloom)
		{ m_enableBloom = enableBloom; }
		
		inline void SetLastPlayedLevel(std::string levelName)
		{ m_lastPlayedLevelName = levelName; }
		inline const std::string& GetLastPlayedLevelName() const
		{ return m_lastPlayedLevelName; }
		
		static Settings instance;
		
	private:
		static QualitySettings ParseQualityString(const std::string& string, QualitySettings def);
		static std::string GetQualityString(QualitySettings quality);
		
		float m_gamma = 1.0f;
		
		bool m_queueFrames = false;
		
		float m_masterVolume = 1;
		float m_musicVolume = 1;
		float m_sfxVolume = 1;
		
		int m_resolutionIndex = -1;
		glm::ivec2 m_resolution{ -1, -1 };
		bool m_isFullscreen = false;
		
		bool m_enableVSync = true;
		
		ResolutionScales m_resolutionScale = ResolutionScales::_100;
		
		QualitySettings m_lightingQuality = QualitySettings::High;
		QualitySettings m_particlesQuality = QualitySettings::High;
		QualitySettings m_postProcessingQuality = QualitySettings::High;
		
		bool m_enableBloom = true;
		
		std::string m_lastPlayedLevelName = "level_1";
	};
}
