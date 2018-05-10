#include "propparser.h"
#include "../parseutils.h"
#include "../../entities/propentity.h"
#include "../../../utils/jsonparseutils.h"

namespace TankGame
{
	std::unique_ptr<Entity> PropParser::ParseEntity(const nlohmann::json& json, const ParseParams& params) const
	{
		std::unique_ptr<PropEntity> entity = std::make_unique<PropEntity>(json["prop_class"].get<std::string>());
		
		auto widthIt = json.find("width");
		if (widthIt != json.end())
			entity->SetSizeX(widthIt->get<float>());
		
		auto solidIt = json.find("solid");
		if (solidIt != json.end())
			entity->SetIsSolid(solidIt->get<bool>());
		
		auto zIt = json.find("z");
		if (zIt != json.end())
			entity->SetZ(zIt->get<float>());
		
		return entity;
	}
}
