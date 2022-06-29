#include "flamethrowerparser.h"
#include "../../entities/flamethrowerentity.h"

#include <nlohmann/json.hpp>

namespace TankGame
{
	std::unique_ptr<Entity> FlameThrowerParser::ParseEntity(const nlohmann::json& json, const ParseParams& params) const
	{
		float flameLength = 2.0f;
		auto flameLengthIt = json.find("flame_length");
		if (flameLengthIt != json.end())
			flameLength = *flameLengthIt;
		
		auto manualControlIt = json.find("manual_control");
		if (manualControlIt != json.end() && manualControlIt->get<bool>())
		{
			FlameThrowerEntity entity = FlameThrowerEntity::CreateManuallyControlled(flameLength);
			
			auto enabledIt = json.find("enabled");
			if (enabledIt != json.end() && enabledIt->get<bool>())
				entity.SetEnabled(true);
			
			return std::make_unique<FlameThrowerEntity>(std::move(entity));
		}
		
		float enabledTime = 2.0f;
		float disabledTime = 2.0f;
		float startTime = 0.0f;
		
		auto enabledTimeIt = json.find("enabled_time");
		if (enabledTimeIt != json.end())
			enabledTime = *enabledTimeIt;
		
		auto disabledTimeIt = json.find("disabled_time");
		if (disabledTimeIt != json.end())
			disabledTime = *disabledTimeIt;
		
		auto startTimeIt = json.find("start_time");
		if (startTimeIt != json.end())
			startTime = *startTimeIt;
		
		return std::make_unique<FlameThrowerEntity>(flameLength, enabledTime, disabledTime, startTime);
	}
}
