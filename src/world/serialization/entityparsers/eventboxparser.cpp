#include "eventboxparser.h"
#include "../../entities/eventbox.h"

#include <nlohmann/json.hpp>

namespace TankGame
{
	std::unique_ptr<Entity> EventBoxParser::ParseEntity(const nlohmann::json& json, const ParseParams& params) const
	{
		std::unique_ptr<EventBox> entity = std::make_unique<EventBox>();
		
		auto playerEnterIt = json.find("player_enter");
		if (playerEnterIt != json.end())
			entity->SetPlayerEnterScript(playerEnterIt->get<std::string>());
		
		auto playerLeaveIt = json.find("player_leave");
		if (playerLeaveIt != json.end())
			entity->SetPlayerLeaveScript(playerLeaveIt->get<std::string>());
		
		return entity;
	}
}
