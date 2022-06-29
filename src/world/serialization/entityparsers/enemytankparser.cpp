#include "enemytankparser.h"
#include "../parseutils.h"
#include "../../entities/enemies/enemytank.h"
#include "../../../utils/jsonparseutils.h"

#include <nlohmann/json.hpp>

namespace TankGame
{
	std::unique_ptr<Entity> EnemyTankParser::ParseEntity(const nlohmann::json& json, const ParseParams& params) const
	{
		Path idlePath;
		for (auto& pathEntry : json["idle_path"])
			idlePath.AddNode(ParseVec2(pathEntry));
		idlePath.Close();
		
		std::unique_ptr<EnemyTank> enemyTank = std::make_unique<EnemyTank>(idlePath);
		
		auto rocketTankIt = json.find("rocket_tank");
		if (rocketTankIt != json.end())
			enemyTank->SetIsRocketTank(rocketTankIt->get<bool>());
		
		auto hasShieldIt = json.find("has_shield");
		if (hasShieldIt != json.end())
			enemyTank->SetHasShield(hasShieldIt->get<bool>());
		
		return enemyTank;
	}
}
