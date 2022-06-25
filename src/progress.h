#pragma once

#include "utils/filesystem.h"

#include <unordered_map>

namespace TankGame
{
	class Progress
	{
	public:
		Progress();
		
		int GetLevelProgress(const std::string& name) const;
		void UpdateLevelProgress(const std::string& name, int newProgress);
		
		void Load();
		
		void Save() const;
		
		inline static Progress& GetInstance()
		{ return s_instance; }
		inline static void SetInstance(Progress instance)
		{ s_instance = std::move(instance); }
		
	private:
		static Progress s_instance;
		
		fs::path m_path;
		
		std::unordered_map<std::string, int> m_levelProgresses;
	};
}
