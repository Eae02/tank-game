#pragma once

#include <glm/glm.hpp>
#include "../rectangle.h"

namespace TankGame
{
	glm::mat3 MapNDCToRectangle(const Rectangle& rectangle);
	
	float ModulateRotation(float input);
	
	glm::vec2 RotateVector(glm::vec2 vector, float amount);
	
	float GetRotationDifference(float rotationA, float rotationB);
	
	template <typename T>
	inline float LengthSquared(T vector)
	{ return glm::dot(vector, vector); }
	
	template <typename T>
	inline T RoundToNextMultiple(T value, T multiple)
	{
		T valModMul = value % multiple;
		return valModMul == 0 ? value : (value + multiple - valModMul);
	}
}
