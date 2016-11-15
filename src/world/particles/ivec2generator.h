#pragma once

#include "../../utils/abstract.h"

#include <glm/glm.hpp>
#include <random>

namespace TankGame
{
	class IVec2Generator : public Abstract
	{
	public:
		virtual glm::vec2 GenerateVec2(std::mt19937& random, float subframeInterpolation) const = 0;
	};
}
