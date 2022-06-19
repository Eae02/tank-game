#pragma once

#include <string>

#include "../entity.h"

namespace TankGame
{
	const class EntityParser* GetEntityParser(const std::string& className);
}
