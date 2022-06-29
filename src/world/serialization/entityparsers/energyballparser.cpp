#include "energyballparser.h"
#include "../../entities/energyball.h"
#include "../../../utils/jsonparseutils.h"

#include <nlohmann/json.hpp>

namespace TankGame
{
	std::unique_ptr<Entity> EnergyBallParser::ParseEntity(const nlohmann::json& json, const ParseParams& params) const
	{
		float damage = 70;
		auto damageIt = json.find("damage");
		if (damageIt != json.end())
			damage = damageIt->get<float>();
		
		return std::make_unique<EnergyBall>(ParseVec2(json["direction"]), damage, *params.m_particlesManager);
	}
}
