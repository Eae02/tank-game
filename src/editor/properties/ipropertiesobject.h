#pragma once

#include "../../utils/abstract.h"

namespace TankGame
{
	class IPropertiesObject : public Abstract
	{
	public:
		virtual void RenderProperties() = 0;
		virtual const char* GetObjectName() const = 0;
	};
}
