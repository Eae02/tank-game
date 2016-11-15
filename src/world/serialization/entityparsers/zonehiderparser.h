#pragma once

#include "entityparser.h"

namespace TankGame
{
	class ZoneHiderParser : public EntityParser
	{
	protected:
		virtual std::unique_ptr<Entity> ParseEntity(const nlohmann::json& json) const override;
	};
}
