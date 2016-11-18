#pragma once

#include "utils/abstract.h"

namespace TankGame
{
	class IASyncOperation : public Abstract
	{
	public:
		virtual void DoWork() = 0;
		virtual void ProcessResult() = 0;
	};
}
