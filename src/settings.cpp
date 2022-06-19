#include "settings.h"
#include "utils/utils.h"
#include "utils/ioutils.h"
#include "level.h"
#include "platform/common.h"

#include <nlohmann/json.hpp>
#include <fstream>

namespace TankGame
{
	Settings Settings::instance;
	
	void Settings::Load(const fs::path& jsonPath, const VideoModes& videoModes)
	{
		nlohmann::json settingsDocument = nlohmann::json::parse(ReadFileContents(jsonPath));
		
		const nlohmann::json& videoEl = settingsDocument["video"];
		
		const nlohmann::json& resolutionEl = videoEl["resolution"];
		m_resolution = { resolutionEl[0].get<int>(), resolutionEl[1].get<int>() };
		m_isFullscreen = videoEl["fullscreen"].get<bool>();
		
		if (!videoModes.resolutions.empty())
		{
			auto resolutionIt = std::find(videoModes.resolutions.begin(), videoModes.resolutions.end(), m_resolution);
			if (resolutionIt == videoModes.resolutions.end())
			{
				m_resolutionIndex = videoModes.defaultResolutionIndex;
				m_resolution = videoModes.resolutions[m_resolutionIndex];
			}
			else
			{
				m_resolutionIndex = resolutionIt - videoModes.resolutions.begin();
			}
		}
		
		m_enableVSync = videoEl["vsync"].get<bool>();
		
		auto gammaIt = videoEl.find("gamma");
		if (gammaIt != videoEl.end())
			m_gamma = gammaIt->get<float>();
		
		const nlohmann::json& qualityEl = videoEl["quality"];
		
		m_lightingQuality = ParseQualityString(qualityEl["lighting"].get<std::string>(), m_lightingQuality);
		m_particlesQuality = ParseQualityString(qualityEl["particles"].get<std::string>(), m_particlesQuality);
		m_postProcessingQuality = ParseQualityString(qualityEl["post"].get<std::string>(), m_postProcessingQuality);
		
		auto resScaleIt = qualityEl.find("resScale");
		if (resScaleIt != qualityEl.end())
			m_resolutionScale = static_cast<ResolutionScales>(resScaleIt->get<int>());
		
		m_enableBloom = qualityEl["bloom"].get<bool>();
		
		auto audioIt = settingsDocument.find("audio");
		if (audioIt != settingsDocument.end())
		{
			m_masterVolume = (*audioIt)["masterVol"].get<float>();
			m_musicVolume = (*audioIt)["musicVol"].get<float>();
			m_sfxVolume = (*audioIt)["sfxVol"].get<float>();
		}
		
		auto lastLevelIt = settingsDocument.find("lastLevel");
		if (lastLevelIt != settingsDocument.end())
		{
			std::string lastPlayedLevelName = *lastLevelIt;
			if (fs::exists(Level::GetLevelsPath() / lastPlayedLevelName))
				m_lastPlayedLevelName = std::move(lastPlayedLevelName);
		}
	}
	
	void Settings::Save(const fs::path& jsonPath) const
	{
		nlohmann::json json;
		
		nlohmann::json videoEl;
		videoEl["fullscreen"] = m_isFullscreen;
		videoEl["resolution"] = { m_resolution.x, m_resolution.y };
		videoEl["vsync"] = m_enableVSync;
		videoEl["gamma"] = m_gamma;
		
		nlohmann::json qualityEl;
		qualityEl["lighting"] = GetQualityString(m_lightingQuality);
		qualityEl["particles"] = GetQualityString(m_particlesQuality);
		qualityEl["post"] = GetQualityString(m_postProcessingQuality);
		qualityEl["resScale"] = static_cast<int>(m_resolutionScale);
		qualityEl["bloom"] = m_enableBloom;
		videoEl["quality"] = qualityEl;
		
		json["video"] = videoEl;
		
		nlohmann::json audioEl;
		audioEl["masterVol"] = m_masterVolume;
		audioEl["musicVol"] = m_musicVolume;
		audioEl["sfxVol"] = m_sfxVolume;
		json["audio"] = audioEl;
		
		json["lastLevel"] = m_lastPlayedLevelName;
		
		std::ofstream stream(jsonPath);
		stream << std::setw(4) << json;
	}
	
	QualitySettings Settings::ParseQualityString(const std::string& string, QualitySettings def)
	{
		if (strcasecmp(string.c_str(), "low") == 0)
			return QualitySettings::Low;
		if (strcasecmp(string.c_str(), "medium") == 0)
			return QualitySettings::Medium;
		if (strcasecmp(string.c_str(), "high") == 0)
			return QualitySettings::High;
		return def;
	}
	
	std::string Settings::GetQualityString(QualitySettings quality)
	{
		switch (quality)
		{
		case QualitySettings::Low:
			return "low";
		case QualitySettings::Medium:
			return "medium";
		case QualitySettings::High:
			return "high";
		default:
			return "unknown";
		}
	}
}
