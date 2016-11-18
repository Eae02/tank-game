#include "propclassloadoperation.h"
#include "propsmanager.h"
#include "../../utils/ioutils.h"

#include <algorithm>

namespace TankGame
{
	void PropClassLoadOperation::DoWork()
	{
		for (const fs::directory_entry& entry : fs::recursive_directory_iterator(m_dirPath))
		{
			if (!fs::is_regular_file(entry.path()) || entry.path().extension() != ".json")
				continue;
			
			fs::path parentPath = entry.path().parent_path();
			
			nlohmann::json json = nlohmann::json::parse(ReadFileContents(entry.path()));
			
			MaybeLoadTexture((parentPath / json["diffuse"].get<std::string>()).string());
			
			auto normalMapEl = json.find("normalMap");
			if (normalMapEl != json.end())
				MaybeLoadTexture((parentPath / normalMapEl->get<std::string>()).string());
			
			m_classesToLoad.emplace_back(std::move(json), std::move(parentPath));
		}
	}
	
	void PropClassLoadOperation::MaybeLoadTexture(const std::string& path)
	{
		if (m_propsManager.m_textures.find(path) != m_propsManager.m_textures.end())
			return;
		
		for (const TextureLoadOperation& texture : m_textureLoadOperations)
		{
			if (texture.GetPath() == path)
				return;
		}
		
		m_textureLoadOperations.emplace_back(path, nullptr);
		m_textureLoadOperations.back().DoWork();
	}
	
	void PropClassLoadOperation::ProcessResult()
	{
		for (const TextureLoadOperation& texture : m_textureLoadOperations)
			m_propsManager.m_textures[texture.GetPath()] = std::make_unique<Texture2D>(texture.CreateTexture());
		
		for (const ClassToLoad& classToLoad : m_classesToLoad)
		{
			PropClass propClass = PropClass::FromJSON(classToLoad.m_json, classToLoad.m_parentPath, m_propsManager);
			
			m_propsManager.m_propClasses.emplace_back(std::make_unique<PropClass>(std::move(propClass)));
		}
		
		m_propsManager.SortPropClasses();
	}
}
