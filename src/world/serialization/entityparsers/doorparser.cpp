#include "doorparser.h"
#include "../../entities/doorentity.h"

namespace TankGame
{
	std::unique_ptr<Entity> DoorParser::ParseEntity(const nlohmann::json& json, const ParseParams& params) const
	{
		std::unique_ptr<DoorEntity> entity = std::make_unique<DoorEntity>();
		
		auto sizeIt = json.find("size");
		if (sizeIt != json.end())
			entity->SetSizeX(sizeIt->get<float>() / 2.0f);
		
		return entity;
	}
}
