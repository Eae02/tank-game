#include "raylightparser.h"
#include "../parseutils.h"
#include "../../lights/raylightentity.h"
#include "../../../utils/jsonparseutils.h"

namespace TankGame
{
	std::unique_ptr<Entity> RayLightParser::ParseEntity(const nlohmann::json& json, const ParseParams& params) const
	{
		std::unique_ptr<RayLightEntity> entity = std::make_unique<RayLightEntity>();
		
		auto intensityIt = json.find("intensity");
		if (intensityIt != json.end())
			entity->SetIntensity(intensityIt->get<float>());
		/*
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
		*/
		auto colorIt = json.find("color");
		if (colorIt != json.end())
			entity->SetColor(ParseColor(*colorIt));
		
		auto lengthIt = json.find("length");
		if (lengthIt != json.end())
			entity->SetLength(lengthIt->get<float>());
		
		Attenuation attenuation;
		
		auto attenuationLinIt = json.find("attenuation_lin");
		if (attenuationLinIt != json.end())
			attenuation.SetLinear(attenuationLinIt->get<float>());
		
		auto attenuationExpIt = json.find("attenuation_exp");
		if (attenuationExpIt != json.end())
			attenuation.SetLinear(attenuationExpIt->get<float>());
		
		return entity;
	}
}
