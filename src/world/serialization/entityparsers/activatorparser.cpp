#include "activatorparser.h"
#include "../../entities/activatorentity.h"

namespace TankGame
{
	std::unique_ptr<Entity> ActivatorParser::ParseEntity(const nlohmann::json& json) const
	{
		bool singleUse = true;
		auto singleUseIt = json.find("single_use");
		if (singleUseIt != json.end())
			singleUse = singleUseIt->get<bool>();
		
		return std::make_unique<ActivatorEntity>(json["activated"].get<std::string>(), singleUse);
	}
	
}
