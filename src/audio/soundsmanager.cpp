#include "soundsmanager.h"
#include "../utils/ioutils.h"

#include <nlohmann/json.hpp>

namespace TankGame
{
	std::unique_ptr<SoundsManager> SoundsManager::s_instance;
	
	SoundsManager::SoundsManager(const fs::path& jsonPath)
	{
		fs::path parentPath = jsonPath.parent_path();
		
		nlohmann::json jsonDocument = nlohmann::json::parse(ReadFileContents(jsonPath));
		for (const nlohmann::json& element : jsonDocument)
		{
			auto nameIt = element.find("name");
			auto srcIt = element.find("src");
			
			if (nameIt == element.end() || srcIt == element.end())
				continue;
			
			fs::path fullPath = parentPath / srcIt->get<std::string>();
			
			m_sounds.emplace(nameIt->get<std::string>(), AudioBuffer::FromOGG(fullPath.string()));
		}
	}
	
	const AudioBuffer& SoundsManager::GetSound(const std::string& name) const
	{
		auto it = m_sounds.find(name);
		if (it == m_sounds.end())
			throw std::runtime_error("Sound '" + name + "' not loaded.");
		return it->second;
	}
}
