#include "eventboxparser.h"
#include "../../entities/eventbox.h"

namespace TankGame
{
	std::unique_ptr<Entity> EventBoxParser::ParseEntity(const nlohmann::json& json) const
	{
		std::unique_ptr<EventBox> entity = std::make_unique<EventBox>();
		
		auto playerEnterIt = json.find("player_enter");
		if (playerEnterIt != json.end())
			entity->SetPlayerEnterEvent(playerEnterIt->get<std::string>());
		
		auto playerLeaveIt = json.find("player_leave");
		if (playerLeaveIt != json.end())
			entity->SetPlayerLeaveEvent(playerLeaveIt->get<std::string>());
		
		return entity;
	}
}
