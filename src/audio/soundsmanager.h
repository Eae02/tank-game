#pragma once

#include "audiobuffer.h"

#include <unordered_map>

namespace TankGame
{
	enum class Sounds
	{
		Blaster = 0,
		Beam = 1
	};
	
	class SoundsManager
	{
	public:
		explicit SoundsManager(const fs::path& jsonPath);
		
		const AudioBuffer& GetSound(const std::string& name) const;
		
		inline static void SetInstance(std::unique_ptr<SoundsManager>&& instance)
		{ s_instance = std::move(instance); }
		inline static SoundsManager& GetInstance()
		{ return *s_instance; }
		
	private:
		static std::unique_ptr<SoundsManager> s_instance;
		
		std::unordered_map<std::string, AudioBuffer> m_sounds;
	};
}
