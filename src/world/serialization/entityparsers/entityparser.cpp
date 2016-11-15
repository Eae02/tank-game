#include "entityparser.h"
#include "../parseutils.h"
#include "../../../utils/jsonparseutils.h"

namespace TankGame
{
	std::unique_ptr<Entity> EntityParser::Parse(const nlohmann::json& json) const
	{
		std::unique_ptr<Entity> parsedEntity = ParseEntity(json);
		
		auto transformIt = json.find("transform");
		if (transformIt != json.end())
			ParseTransform(*transformIt, parsedEntity->GetTransform());
		
		return parsedEntity;
	}
}
