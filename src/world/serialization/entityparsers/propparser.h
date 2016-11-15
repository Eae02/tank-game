#pragma once

#include "entityparser.h"

namespace TankGame
{
	class PropParser : public EntityParser
	{
	protected:
		virtual std::unique_ptr<Entity> ParseEntity(const nlohmann::json& json) const final override;
	};
}
