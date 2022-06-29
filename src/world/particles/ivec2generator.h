#pragma once

#include "../../utils/abstract.h"

#include <pcg_random.hpp>

namespace TankGame
{
	class IVec2Generator : public Abstract
	{
	public:
		using RNG = pcg64_fast;
		
		virtual glm::vec2 GenerateVec2(RNG& random, float subframeInterpolation) const = 0;
	};
}
