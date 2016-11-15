#pragma once

#include "entityparser.h"

namespace TankGame
{
	class RocketTurretParser : public EntityParser
	{
	protected:
		virtual std::unique_ptr<Entity> ParseEntity(const nlohmann::json& json) const override;
	};
}
