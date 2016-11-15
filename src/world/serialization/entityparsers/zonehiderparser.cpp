#include "zonehiderparser.h"
#include "../../entities/zonehiderentity.h"

namespace TankGame
{
	std::unique_ptr<Entity> ZoneHiderParser::ParseEntity(const nlohmann::json& json) const
	{
		std::string name;
		
		auto nameIt = json.find("zone_name");
		if (nameIt != json.end())
			name = nameIt->get<std::string>();
		
		return std::make_unique<ZoneHiderEntity>(name);
	}
}
