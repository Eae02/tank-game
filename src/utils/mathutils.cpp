#include "mathutils.h"

namespace TankGame
{
	glm::mat3 MapNDCToRectangle(const Rectangle& rectangle)
	{
		return glm::transpose(glm::mat3(
				rectangle.w / 2.0f, 0.0f, rectangle.CenterX(),
				0.0f, rectangle.h / 2.0f, rectangle.CenterY(),
				0.0f, 0.0f, 1.0f
		));
	}
	
	glm::vec2 RotateVector(glm::vec2 vector, float amount)
	{
		float sinR = std::sin(amount);
		float cosR = std::cos(amount);
		
		return glm::vec2(
				cosR * vector.x - sinR * vector.y,
				sinR * vector.x + cosR * vector.y
		);
	}
	
	float ModulateRotation(float input)
	{
		return input - std::floor(input / glm::two_pi<float>()) * glm::two_pi<float>();
	}
	
	float GetRotationDifference(float rotationA, float rotationB)
	{
		return ModulateRotation(rotationB - rotationA + glm::pi<float>()) - glm::pi<float>();
	}
}
