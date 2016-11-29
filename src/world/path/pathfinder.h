#pragma once

#include <glm/glm.hpp>

namespace TankGame
{
	bool FindPath(const class TileGrid& tileGrid, const class TileGridMaterial& material,
	              glm::vec2 start, glm::vec2 end, class Path& pathOut, float radius);
	
	bool FindPath(const class GameWorld& gameWorld, glm::vec2 start, glm::vec2 end, class Path& pathOut, float radius);
}
