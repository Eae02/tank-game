#pragma once

#include "../../iasyncoperation.h"
#include "../../utils/filesystem.h"
#include "../../graphics/textureloadoperation.h"

#include <json.hpp>

namespace TankGame
{
	class PropClassLoadOperation : public IASyncOperation
	{
	public:
		inline PropClassLoadOperation(class PropsManager& propsManager, fs::path dirPath)
		    : m_propsManager(propsManager), m_dirPath(dirPath) { }
		
		virtual void DoWork() override;
		virtual void ProcessResult() override;
		
	private:
		void MaybeLoadTexture(const std::string& path);
		
		struct ClassToLoad
		{
			nlohmann::json m_json;
			fs::path m_parentPath;
			
			inline ClassToLoad(nlohmann::json json, fs::path parentPath)
			    : m_json(json), m_parentPath(parentPath) { }
		};
		
		class PropsManager& m_propsManager;
		
		fs::path m_dirPath;
		
		std::vector<TextureLoadOperation> m_textureLoadOperations;
		std::vector<ClassToLoad> m_classesToLoad;
	};
}
