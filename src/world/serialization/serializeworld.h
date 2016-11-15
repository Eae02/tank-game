#pragma once

#include <ostream>

namespace TankGame
{
	void SerializeWorld(const class GameWorld& gameWorld, std::ostream& stream);
	
	void WriteEmptyWorld(const std::string& name, uint32_t width, uint32_t height, std::ostream& stream);
}
