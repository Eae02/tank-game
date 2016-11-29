#include "lightstripparser.h"
#include "../../path/path.h"
#include "../../entities/lightstripentity.h"
#include "../../../utils/jsonparseutils.h"

namespace TankGame
{
	std::unique_ptr<Entity> LightStripParser::ParseEntity(const nlohmann::json& json) const
	{
		glm::vec3 color(1.0f);
		auto colorIt = json.find("color");
		if (colorIt != json.end())
			color = ParseColor(*colorIt);
		
		float glowStrength = 5.0f;
		auto glowStrengthIt = json.find("glow_strength");
		if (glowStrengthIt != json.end())
			glowStrength = glowStrengthIt->get<float>();
		
		float radius = 0.05f;
		auto radiusIt = json.find("radius");
		if (radiusIt != json.end())
			radius = radiusIt->get<float>();
		
		std::string activateEvent;
		auto activateEventNameIt = json.find("activate_event");
		if (activateEventNameIt != json.end())
			activateEvent = activateEventNameIt->get<std::string>();
		
		auto activatedColorIt = json.find("activated_color");
		glm::vec3 activatedColor = color;
		if (activatedColorIt != json.end())
			activatedColor = ParseColor(*activatedColorIt);
		
		std::unique_ptr<LightStripEntity> entity = std::make_unique<LightStripEntity>(color, glowStrength, radius,
		                                                                              std::move(activateEvent),
		                                                                              activatedColor);
		
		Path path;
		for (const nlohmann::json& nodeEl : json["nodes"])
			path.AddNode(ParseVec2(nodeEl));
		entity->SetPath(path);
		
		return entity;
	}
	
}
