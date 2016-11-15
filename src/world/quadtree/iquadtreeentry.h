#pragma once

#include "../../rectangle.h"
#include "../../utils/abstract.h"

namespace TankGame
{
	class IQuadTreeEntry : public Abstract
	{
	public:
		virtual Rectangle GetBoundingRectangle() const = 0;
	};
}
