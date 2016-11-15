#include "aiutils.h"
#include "../utils/mathutils.h"

#include <glm/gtc/constants.hpp>
#include <cmath>

namespace TankGame
{
	void WalkPath(float dt, const Path& path, float& progress, Transform& transform,
	              float movementSpeed, float turnRate, bool modulate)
	{
		float newPathProgress = progress + movementSpeed * dt;
		if (modulate)
			newPathProgress = path.ModulateProgress(newPathProgress);
		auto newPosition = path.GetPositionFromProgress(newPathProgress);
		
		float targetRotation = glm::half_pi<float>() + std::atan2(newPosition.m_forward.y, newPosition.m_forward.x);
		
		float rotationDiff = GetRotationDifference(transform.GetRotation(), targetRotation);
		
		if (std::abs(rotationDiff) < glm::radians(15.0f))
		{
			transform.SetPosition(newPosition.m_position);
			progress = newPathProgress;
		}
		
		float deltaRotate = glm::min(turnRate * dt, std::abs(rotationDiff));
		transform.Rotate(deltaRotate * glm::sign(rotationDiff));
	}
}
