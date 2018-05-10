#pragma once

#include <string>
#include <json.hpp>
#include <glm/glm.hpp>
#include "../transform.h"

namespace TankGame
{
	glm::vec2 ParseVec2(const nlohmann::json& json);
	glm::vec3 ParseColor(const nlohmann::json& json);
	
	void ParseTransform(const nlohmann::json& json, Transform& transform);
	
	inline Transform ParseTransform(const nlohmann::json& json)
	{
		Transform transform;
		ParseTransform(json, transform);
		return transform;
	}
}
