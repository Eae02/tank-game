#pragma once


#include "path.h"
#include "../../graphics/tilegridmaterial.h"

namespace TankGame
{
	bool FindPath(const class TileGrid& tileGrid, const TileGridMaterial& material,
	              glm::vec2 start, glm::vec2 end, Path& pathOut, float radius);
	
	bool FindPath(const class GameWorld& gameWorld, glm::vec2 start, glm::vec2 end, Path& pathOut, float radius);
}
