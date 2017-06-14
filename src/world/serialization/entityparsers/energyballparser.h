#pragma once

#include "entityparser.h"

namespace TankGame
{
	class EnergyBallParser : public EntityParser
	{
	public:
		virtual std::unique_ptr<Entity> ParseEntity(const nlohmann::json& json, const ParseParams& params) const override;
	};
}
