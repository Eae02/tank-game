﻿#include "progress.h"
#include "utils/ioutils.h"
#include "platform/common.h"

#include <nlohmann/json.hpp>
#include <fstream>

namespace TankGame
{
	Progress Progress::s_instance;
	
	Progress::Progress() : m_path(dataDirectoryPath / "progress.json") { }
	
	void Progress::Load()
	{
		if (fs::exists(m_path))
		{
			nlohmann::json json = nlohmann::json::parse(ReadFileContents(m_path));
			
			for (nlohmann::json::iterator it = json.begin(); it != json.end(); ++it)
			{
				if (it.value().is_number_integer())
					m_levelProgresses.emplace(it.key(), it.value().get<int>());
			}
		}
	}
	
	int Progress::GetLevelProgress(const std::string& name) const
	{
		auto it = m_levelProgresses.find(name);
		if (it == m_levelProgresses.end())
			return -1;
		return it->second;
	}
	
	void Progress::UpdateLevelProgress(const std::string& name, int newProgress)
	{
		auto it = m_levelProgresses.find(name);
		
		if (it == m_levelProgresses.end())
		{
			m_levelProgresses.emplace(name, newProgress);
			Save();
		}
		else if (newProgress > it->second)
		{
			it->second = newProgress;
			Save();
		}
	}
	
	void Progress::Save() const
	{
		nlohmann::json json;
		
		for (const auto& levelProgress : m_levelProgresses)
		{
			json[levelProgress.first] = levelProgress.second;
		}
		
		std::ofstream stream(m_path);
		stream << json;
		stream.close();
		SyncFileSystem();
	}
}
