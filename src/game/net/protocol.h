#pragma once

#include <cstdint>
#include "message.h"

namespace TankGame
{
	constexpr uint16_t SERVER_PORT = 15051;
	constexpr uint32_t MAX_NAME_LEN = 256;
	
#pragma pack(push, 1)
	struct MessageHeader
	{
		MessageID id;
		uint32_t size;
	};
#pragma pack(pop)
};
