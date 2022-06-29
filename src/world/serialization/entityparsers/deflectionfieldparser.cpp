#include "deflectionfieldparser.h"
#include "../../entities/deflectionfieldentity.h"

#include <nlohmann/json.hpp>

namespace TankGame
{
	std::unique_ptr<Entity> DeflectionFieldParser::ParseEntity(const nlohmann::json& json, const ParseParams& params) const
	{
		std::unique_ptr<DeflectionFieldEntity> entity = std::make_unique<DeflectionFieldEntity>();
		
		auto deflectEnemyIt = json.find("deflect_enemy");
		if (deflectEnemyIt != json.end())
			entity->SetDeflectEnemy(deflectEnemyIt->get<bool>());
		
		auto lengthIt = json.find("length");
		if (lengthIt != json.end())
			entity->SetLength(lengthIt->get<float>());
		
		return entity;
	}
}
