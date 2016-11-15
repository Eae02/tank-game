#include "doorparser.h"
#include "../../entities/doorentity.h"

namespace TankGame
{
	std::unique_ptr<Entity> DoorParser::ParseEntity(const nlohmann::json& json) const
	{
		std::string openEventName;
		auto openEventIt = json.find("open_event");
		if (openEventIt != json.end())
			openEventName = openEventIt->get<std::string>();
		
		std::unique_ptr<DoorEntity> entity = std::make_unique<DoorEntity>(std::move(openEventName));
		
		auto sizeIt = json.find("size");
		if (sizeIt != json.end())
			entity->SetSizeX(sizeIt->get<float>() / 2.0f);
		
		return entity;
	}
}
