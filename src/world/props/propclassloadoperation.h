#pragma once

#include "propsmanager.h"
#include "../../graphics/textureloadoperation.h"

#include <nlohmann/json.hpp>
#include <future>

namespace TankGame
{
	class PropClassLoadOperation
	{
	public:
		static std::future<PropClassLoadOperation> Load(fs::path dirPath);
		
		std::unique_ptr<PropsManager> FinishLoading();
		
	private:
		PropClassLoadOperation() = default;
		
		void MaybeLoadTexture(const std::string& path);
		
		struct ClassToLoad
		{
			nlohmann::json m_json;
			fs::path m_parentPath;
			
			inline ClassToLoad(nlohmann::json json, fs::path parentPath)
			    : m_json(json), m_parentPath(parentPath) { }
		};
		
		std::unique_ptr<PropsManager> m_propsManager;
		
		fs::path m_dirPath;
		
		std::vector<TextureLoadOperation> m_textureLoadOperations;
		std::vector<ClassToLoad> m_classesToLoad;
	};
}
