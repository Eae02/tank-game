#include "entityparser.h"
#include "../parseutils.h"
#include "../../../utils/jsonparseutils.h"

#include <nlohmann/json.hpp>

namespace TankGame
{
	std::unique_ptr<Entity> EntityParser::Parse(const nlohmann::json& json, const ParseParams& params) const
	{
		std::unique_ptr<Entity> parsedEntity = ParseEntity(json, params);
		
		auto transformIt = json.find("transform");
		if (transformIt != json.end())
			ParseTransform(*transformIt, parsedEntity->GetTransform());
		
		return parsedEntity;
	}
}
