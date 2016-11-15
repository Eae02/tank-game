#include "propparser.h"
#include "../parseutils.h"
#include "../../entities/propentity.h"
#include "../../../utils/jsonparseutils.h"

namespace TankGame
{
	std::unique_ptr<Entity> PropParser::ParseEntity(const nlohmann::json& json) const
	{
		std::unique_ptr<PropEntity> entity = std::make_unique<PropEntity>(json["prop_class"].get<std::string>());
		
		auto widthIt = json.find("width");
		if (widthIt != json.end())
		{
			entity->SetSizeX(widthIt->get<float>());
		}
		
		return entity;
	}
}
