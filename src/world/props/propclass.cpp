#include "propclass.h"
#include "propsmanager.h"
#include "../serialization/parseutils.h"
#include "../../utils/ioutils.h"
#include "../../utils/jsonparseutils.h"
#include "../../graphics/defaultnormalmap.h"

namespace TankGame
{
	PropClass PropClass::FromJSON(const nlohmann::json& json, const fs::path& parentPath, PropsManager& propsManager)
	{
		const Texture2D* diffuse = propsManager.GetTexture((parentPath / json["diffuse"].get<std::string>()).string());
		
		const Texture2D* normalMap = nullptr;
		auto normalMapIt = json.find("normalMap");
		if (normalMapIt != json.end())
			normalMap = propsManager.GetTexture((parentPath / normalMapIt->get<std::string>()).string());
		
		float specIntensity = json["specular"]["intensity"].get<float>();
		float specExponent = json["specular"]["exponent"].get<float>();
		
		glm::vec4 shade(1.0f);
		
		auto shadeIt = json.find("shade");
		if (shadeIt != json.end())
			shade = glm::vec4(ParseColor(*shadeIt), 1.0f);
		
		auto opacityIt = json.find("opacity");
		if (opacityIt != json.end())
			shade.a = opacityIt->get<float>();
		
		PropClass propClass({ *diffuse, normalMap == nullptr ? GetDefaultNormalMap() : *normalMap,
		                      specIntensity, specExponent, shade });
		
		auto decalIt = json.find("decal");
		if (decalIt != json.end())
			propClass.m_isDecal = decalIt->get<bool>();
		
		propClass.m_textureWidth = diffuse->GetWidth();
		propClass.m_textureHeight = diffuse->GetHeight();
		
		propClass.m_name = json["name"].get<std::string>();
		
		return propClass;
	}
}
