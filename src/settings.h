#pragma once

#include "utils/filesystem.h"

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include <json.hpp>

namespace TankGame
{
	enum class QualitySettings
	{
		Low = 0,
		Medium = 1,
		High = 2,
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
		Settings();
		explicit Settings(const fs::path& jsonPath);
		
		void Save(const fs::path& jsonPath) const;
		
		inline glm::ivec2 GetResolution() const
		{ return m_resolution; }
		inline void SetResolution(glm::ivec2 resolution)
		{ m_resolution = resolution; }
		
		inline bool IsFullscreen() const
		{ return m_isFullscreen; }
		inline void SetIsFullscreen(bool isFullscreen)
		{ m_isFullscreen = isFullscreen; }
		
		inline bool EnableVSync() const
		{ return m_enableVSync; }
		inline void SetEnableVSync(bool enableVSync)
		{ m_enableVSync = enableVSync; }
		
		inline int GetVideoModeIndex() const
		{ return m_videoModeIndex; }
		inline void SetVideoModeIndex(int videoModeIndex)
		{ m_videoModeIndex = videoModeIndex; }
		
		static inline void SetInstance(Settings settings)
		{ s_instance = std::move(settings); }
		static inline Settings& GetInstance()
		{ return s_instance; }
		
		static void DetectVideoModes();
		
		static inline int GetResolutionsCount()
		{ return s_resolutions.size(); }
		static inline glm::ivec2 GetResolution(int n)
		{ return s_resolutions[n]; }
		
		static inline int GetDefaultResolutionIndex()
		{ return s_defaultResolutionIndex; }
		
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
		
		inline int GetForwardButton() const
		{ return m_forwardButton; }
		inline int GetBackButton() const
		{ return m_backButton; }
		inline int GetLeftButton() const
		{ return m_leftButton; }
		inline int GetRightButton() const
		{ return m_rightButton; }
		inline int GetInteractButton() const
		{ return m_interactButton; }
		inline int GetFireButton() const
		{ return m_fireButton; }
		
		inline void SetForwardButton(int button)
		{ m_forwardButton = button; }
		inline void SetBackButton(int button)
		{ m_backButton = button; }
		inline void SetLeftButton(int button)
		{ m_leftButton = button; }
		inline void SetRightButton(int button)
		{ m_rightButton = button; }
		inline void SetInteractButton(int button)
		{ m_interactButton = button; }
		inline void SetFireButton(int button)
		{ m_fireButton = button; }
		
		inline QualitySettings GetLightingQuality() const
		{ return m_lightingQuality; }
		inline QualitySettings GetParticleQuality() const
		{ return m_particlesQuality; }
		inline QualitySettings GetPostProcessingQuality() const
		{ return m_postProcessingQuality; }
		inline bool EnableBloom() const
		{ return m_enableBloom; }
		
		inline void SetLightingQuality(QualitySettings lightingQuality)
		{ m_lightingQuality = lightingQuality; }
		inline void SetParticleQuality(QualitySettings particleQuality)
		{ m_particlesQuality = particleQuality; }
		inline void SetPostProcessingQuality(QualitySettings postProcessingQuality)
		{ m_postProcessingQuality = postProcessingQuality; }
		
	private:
		static void MaybeParseButtonFromJSON(const nlohmann::json& json, const std::string& elementName, int& out);
		static int ParseButtonString(const std::string& string);
		static std::string GetButtonString(int button);
		
		static QualitySettings ParseQualityString(const std::string& string, QualitySettings def);
		static std::string GetQualityString(QualitySettings quality);
		
		static Settings s_instance;
		
		static std::vector<glm::ivec2> s_resolutions;
		static int s_defaultResolutionIndex;
		
		int m_videoModeIndex;
		
		int m_forwardButton = GLFW_KEY_W;
		int m_backButton = GLFW_KEY_S;
		int m_leftButton = GLFW_KEY_A;
		int m_rightButton = GLFW_KEY_D;
		int m_interactButton = GLFW_KEY_E;
		int m_fireButton = GLFW_MOUSE_BUTTON_1;
		
		float m_masterVolume = 1;
		float m_musicVolume = 1;
		float m_sfxVolume = 1;
		
		glm::ivec2 m_resolution{ 800, 500 };
		bool m_isFullscreen = true;
		
		bool m_enableVSync = true;
		
		QualitySettings m_lightingQuality = QualitySettings::High;
		QualitySettings m_particlesQuality = QualitySettings::High;
		QualitySettings m_postProcessingQuality = QualitySettings::High;
		
		bool m_enableBloom = true;
	};
}
