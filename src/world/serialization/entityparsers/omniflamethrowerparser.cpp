#include "omniflamethrowerparser.h"
#include "../../entities/omniflamethrower.h"

#include <nlohmann/json.hpp>

namespace TankGame
{
	std::unique_ptr<Entity> OmniFlameThrowerParser::ParseEntity(const nlohmann::json& json, const ParseParams& params) const
	{
		int count = 2;
		float flameLength = 2.0f;
		
		auto flameLengthIt = json.find("flame_length");
		if (flameLengthIt != json.end())
			flameLength = *flameLengthIt;
		
		auto countIt = json.find("count");
		if (countIt != json.end())
			count = *countIt;
		
		return std::make_unique<OmniFlameThrower>(count, flameLength);
	}
}
