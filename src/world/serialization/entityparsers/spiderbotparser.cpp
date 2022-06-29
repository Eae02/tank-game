#include "spiderbotparser.h"
#include "../../entities/enemies/spiderbot.h"

#include <nlohmann/json.hpp>

namespace TankGame
{
	std::unique_ptr<Entity> SpiderBotParser::ParseEntity(const nlohmann::json& json, const ParseParams& params) const
	{
		std::unique_ptr<SpiderBot> entity = std::make_unique<SpiderBot>();
		
		return entity;
	}
}
