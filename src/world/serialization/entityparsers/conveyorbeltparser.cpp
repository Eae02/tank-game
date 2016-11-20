#include "conveyorbeltparser.h"
#include "../../entities/conveyorbeltentity.h"

namespace TankGame
{
	std::unique_ptr<Entity> ConveyorBeltParser::ParseEntity(const nlohmann::json& json) const
	{
		float speed = 1;
		auto speedIt = json.find("speed");
		if (speedIt != json.end())
			speed = speedIt->get<float>();
		
		std::unique_ptr<ConveyorBeltEntity> entity = std::make_unique<ConveyorBeltEntity>(speed);
		
		auto enabledIt = json.find("enabled");
		if (enabledIt != json.end())
			entity->SetEnabled(enabledIt->get<bool>());
		
		return entity;
	}
}
