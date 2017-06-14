#pragma once

#include "entityparser.h"

namespace TankGame
{
	class ActivatorParser : public EntityParser
	{
	protected:
		virtual std::unique_ptr<Entity> ParseEntity(const nlohmann::json& json, const ParseParams& params) const override;
	};
}
