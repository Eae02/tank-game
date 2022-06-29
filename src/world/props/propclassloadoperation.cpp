#include "propclassloadoperation.h"
#include "propsmanager.h"
#include "../../asyncworklist.h"
#include "../../utils/ioutils.h"

namespace TankGame
{
	std::future<PropClassLoadOperation> PropClassLoadOperation::Load(fs::path dirPath)
	{
		return std::async(LOADING_LAUNCH_POLICY, [path=std::move(dirPath)]
		{
			PropClassLoadOperation op;
			op.m_propsManager = std::make_unique<PropsManager>();
			for (const fs::directory_entry& entry : fs::recursive_directory_iterator(path))
			{
				if (!fs::is_regular_file(entry.path()) || entry.path().extension() != ".json")
					continue;
				
				fs::path parentPath = entry.path().parent_path();
				
				nlohmann::json json = nlohmann::json::parse(ReadFileContents(entry.path()));
				
				op.MaybeLoadTexture((parentPath / json["diffuse"].get<std::string>()).string());
				
				auto normalMapEl = json.find("normalMap");
				if (normalMapEl != json.end())
				{
					op.MaybeLoadTexture((parentPath / normalMapEl->get<std::string>()).string());
				}
				
				op.m_classesToLoad.emplace_back(std::move(json), std::move(parentPath));
			}
			return op;
		});
	}
		
	std::unique_ptr<PropsManager> PropClassLoadOperation::FinishLoading()
	{
		for (const TextureLoadOperation& texture : m_textureLoadOperations)
		{
			m_propsManager->m_textures[texture.GetPath().string()] = std::make_unique<Texture2D>(texture.CreateTexture());
		}
		
		for (const ClassToLoad& classToLoad : m_classesToLoad)
		{
			PropClass propClass = PropClass::FromJSON(classToLoad.m_json, classToLoad.m_parentPath, *m_propsManager);
			
			m_propsManager->m_propClasses.emplace_back(std::make_unique<PropClass>(std::move(propClass)));
		}
		
		m_propsManager->SortPropClasses();
		
		return std::move(m_propsManager);
	}
	
	void PropClassLoadOperation::MaybeLoadTexture(const std::string& path)
	{
		if (m_propsManager->m_textures.find(path) != m_propsManager->m_textures.end())
			return;
		
		for (const TextureLoadOperation& texture : m_textureLoadOperations)
		{
			if (texture.GetPath() == path)
				return;
		}
		
		m_textureLoadOperations.push_back(TextureLoadOperation::Load(path, 4));
	}
}
