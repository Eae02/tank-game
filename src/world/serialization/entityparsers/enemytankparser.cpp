#include "enemytankparser.h"
#include "../parseutils.h"
#include "../../entities/enemies/enemytank.h"
#include "../../../utils/jsonparseutils.h"

namespace TankGame
{
	std::unique_ptr<Entity> EnemyTankParser::ParseEntity(const nlohmann::json& json) const
	{
		Path idlePath;
		for (auto& pathEntry : json["idle_path"])
			idlePath.AddNode(ParseVec2(pathEntry));
		idlePath.Close();
		
		std::unique_ptr<EnemyTank> enemyTank = std::make_unique<EnemyTank>(idlePath);
		
		auto detectEventIt = json.find("detect_event");
		if (detectEventIt != json.end())
			enemyTank->SetDetectPlayerEventName(detectEventIt->get<std::string>());
		
		auto rocketTankIt = json.find("rocket_tank");
		if (rocketTankIt != json.end())
			enemyTank->SetIsRocketTank(rocketTankIt->get<bool>());
		
		return enemyTank;
	}
}
