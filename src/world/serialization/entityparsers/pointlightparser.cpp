#include "pointlightparser.h"
#include "../parseutils.h"
#include "../../lights/pointlightentity.h"
#include "../../../utils/jsonparseutils.h"

namespace TankGame
{
	std::unique_ptr<Entity> PointLightParser::ParseEntity(const nlohmann::json& json, const ParseParams& params) const
	{
		std::unique_ptr<PointLightEntity> entity = std::make_unique<PointLightEntity>();
		
		auto intensityIt = json.find("intensity");
		if (intensityIt != json.end())
			entity->SetIntensity(intensityIt->get<float>());
		
		auto flickersIt = json.find("flickers");
		if (flickersIt != json.end())
			entity->SetFlickers(flickersIt->get<bool>());
		
		auto enabledIt = json.find("enabled");
		if (enabledIt != json.end())
			entity->SetEnabled(enabledIt->get<bool>());
		
		auto shadowsIt = json.find("shadows");
		if (shadowsIt != json.end())
		{
			std::string shadowsStr = shadowsIt->get<std::string>();
			
			if (shadowsStr == "none")
				entity->SetShadowMode(EntityShadowModes::None);
			else if (shadowsStr == "static")
				entity->SetShadowMode(EntityShadowModes::Static);
			else if (shadowsStr == "dynamic")
				entity->SetShadowMode(EntityShadowModes::Dynamic);
			else
				throw std::runtime_error("Invalid shadow mode: '" + shadowsStr + "'.");
		}
		
		auto colorIt = json.find("color");
		if (colorIt != json.end())
			entity->SetColor(ParseColor(*colorIt));
		
		Attenuation attenuation;
		
		auto attenuationLinIt = json.find("attenuation_lin");
		if (attenuationLinIt != json.end())
			attenuation.SetLinear(attenuationLinIt->get<float>());
		
		auto attenuationExpIt = json.find("attenuation_exp");
		if (attenuationExpIt != json.end())
			attenuation.SetExponent(attenuationExpIt->get<float>());
		
		entity->SetAttenuation(attenuation);
		
		return entity;
	}
}
