#pragma once

#include <cstdint>

namespace TankGame
{
	enum class ConnectionResponse : uint8_t
	{
		OK         = 0,
		ServerFull = 1,
		NameTaken  = 2
	};
};
