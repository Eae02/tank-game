#pragma once

#include "../world/path.h"
#include "../transform.h"

namespace TankGame
{
	void WalkPath(float dt, const Path& path, float& progress, Transform& transform,
	              float movementSpeed, float turnRate, bool modulate = false);
}
