#pragma once

#include "../iclassparser.h"
#include "../../entity.h"

namespace TankGame
{
	class EntityParser : public IClassParser<Entity>
	{
	public:
		virtual std::unique_ptr<Entity> Parse(const nlohmann::json& json) const final override;
		
	protected:
		virtual std::unique_ptr<Entity> ParseEntity(const nlohmann::json& json) const = 0;
	};
}
