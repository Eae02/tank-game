#include "ammoboxparser.h"
#include "../../entities/ammoboxentity.h"

#include <nlohmann/json.hpp>

namespace TankGame
{
	std::unique_ptr<Entity> AmmoBoxParser::ParseEntity(const nlohmann::json& json, const ParseParams& params) const
	{
		std::unique_ptr<AmmoBoxEntity> entity = std::make_unique<AmmoBoxEntity>();
		
		auto ammoTypesIt = json.find("ammoTypes");
		if (ammoTypesIt != json.end())
		{
			for (size_t i = 0; i < std::min<size_t>(SPECIAL_WEAPONS_COUNT, ammoTypesIt->size()); i++)
				entity->SetHasAmmoType(static_cast<SpecialWeapons>(i), (*ammoTypesIt)[i].get<bool>());
		}
		
		return entity;
	}
}
