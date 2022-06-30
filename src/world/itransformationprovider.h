#pragma once

#include "../utils/abstract.h"

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
