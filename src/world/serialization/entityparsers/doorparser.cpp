#include "doorparser.h"
#include "../../entities/doorentity.h"

namespace TankGame
{
	std::unique_ptr<Entity> DoorParser::ParseEntity(const nlohmann::json& json) const
	{
		std::vector<std::string> openEventNames;
		
		auto openEventIt = json.find("open_event");
		if (openEventIt != json.end())
		{
			if (openEventIt->is_array())
			{
				openEventNames.resize(openEventIt->size());
				for (size_t i = 0; i < openEventIt->size(); i++)
					openEventNames[i] = openEventIt->operator[](i);
			}
			else
			{
				openEventNames.push_back(openEventIt->get<std::string>());
			}
		}
		
		std::unique_ptr<DoorEntity> entity = std::make_unique<DoorEntity>(openEventNames.data(), openEventNames.size());
		
		auto sizeIt = json.find("size");
		if (sizeIt != json.end())
			entity->SetSizeX(sizeIt->get<float>() / 2.0f);
		
		return entity;
	}
}
