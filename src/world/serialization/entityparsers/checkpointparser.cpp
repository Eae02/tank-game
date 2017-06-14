#include "checkpointparser.h"
#include "../../entities/checkpointentity.h"

namespace TankGame
{
	std::unique_ptr<Entity> CheckpointParser::ParseEntity(const nlohmann::json& json, const ParseParams& params) const
	{
		std::unique_ptr<CheckpointEntity> entity = std::make_unique<CheckpointEntity>();
		
		auto indexIt = json.find("index");
		if (indexIt != json.end())
			entity->SetCheckpointIndex(indexIt->get<int>());
		
		auto lengthIt = json.find("length");
		if (lengthIt != json.end())
			entity->SetLength(lengthIt->get<float>());
		
		return entity;
	}
}
