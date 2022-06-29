#pragma once

#include "../../entity.h"

#include <nlohmann/json_fwd.hpp>

namespace TankGame
{
	class EntityParser
	{
	public:
		struct ParseParams
		{
			class ParticlesManager* m_particlesManager;
		};
		
		std::unique_ptr<Entity> Parse(const nlohmann::json& json, const ParseParams& params) const;
		
	protected:
		virtual std::unique_ptr<Entity> ParseEntity(const nlohmann::json& json, const ParseParams& params) const = 0;
	};
}
