#pragma once

#include "../utils/abstract.h"

namespace TankGame
{
	class IMainRenderer : public Abstract
	{
	public:
		virtual void DoDamageFlash() = 0;
		virtual void SetBlurAmount(float blurAmount) = 0;
	};
}
