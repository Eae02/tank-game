#pragma once

#include "entityparser.h"

namespace TankGame
{
	//Entity parser for entities with no saved settings (other than transform)
	template <typename EntityTp>
	class NullEntityParser : public EntityParser
	{
	protected:
		virtual std::unique_ptr<Entity> ParseEntity(const nlohmann::json& json) const final override
		{
			return std::make_unique<EntityTp>();
		}
	};
}
