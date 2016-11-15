#include "propclass.h"
#include "propsmanager.h"
#include "../serialization/parseutils.h"
#include "../../utils/ioutils.h"
#include "../../utils/jsonparseutils.h"
#include "../../graphics/defaultnormalmap.h"

#include <json.hpp>

namespace TankGame
{
	PropClass PropClass::FromJSON(const fs::path& path, PropsManager& propsManager)
	{
		fs::path parentPath = path.parent_path();
		
		nlohmann::json jsonDoc = nlohmann::json::parse(ReadFileContents(path));
		
		std::shared_ptr<Texture2D> diffuse = propsManager.GetTexture((parentPath / jsonDoc["diffuse"].get<std::string>()).string());
		
		std::shared_ptr<Texture2D> normalMap = [&] () -> std::shared_ptr<Texture2D>
		{
			auto normalMapIt = jsonDoc.find("normalMap");
			if (normalMapIt == jsonDoc.end())
				return nullptr;
			return propsManager.GetTexture((parentPath / normalMapIt->get<std::string>()).string());
		}();
		
		float specIntensity = jsonDoc["specular"]["intensity"].get<float>();
		float specExponent = jsonDoc["specular"]["exponent"].get<float>();
		
		glm::vec4 shade(1.0f);
		
		auto shadeIt = jsonDoc.find("shade");
		if (shadeIt != jsonDoc.end())
			shade = glm::vec4(ParseColor(*shadeIt), 1.0f);
		
		auto opacityIt = jsonDoc.find("opacity");
		if (opacityIt != jsonDoc.end())
			shade.a = opacityIt->get<float>();
		
		PropClass propClass({ *diffuse, normalMap == nullptr ? GetDefaultNormalMap() : *normalMap,
		                      specIntensity, specExponent, shade });
		
		propClass.m_diffuse = std::move(diffuse);
		propClass.m_normalMap = std::move(normalMap);
		
		propClass.m_name = jsonDoc["name"].get<std::string>();
		
		return propClass;
	}
}
