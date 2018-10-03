#pragma once

#include "../utils/abstract.h"

#include <glm/glm.hpp>

namespace TankGame
{
	class ITransformationProvider : public Abstract
	{
	public:
		virtual const class Transform& GetTransform() const = 0;
	};
	
	class ILastFrameTransformProvider : public Abstract
	{
	public:
		virtual const class Transform& GetLastFrameTransform() const = 0;
	};
}
