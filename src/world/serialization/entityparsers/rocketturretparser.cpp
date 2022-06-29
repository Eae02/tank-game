#include "rocketturretparser.h"
#include "../../entities/enemies/rocketturret.h"

#include <nlohmann/json.hpp>

namespace TankGame
{
	std::unique_ptr<Entity> RocketTurretParser::ParseEntity(const nlohmann::json& json, const ParseParams& params) const
	{
		float minRotation = -glm::half_pi<float>();
		float maxRotation = glm::half_pi<float>();
		float rotationSpeed = 1.0f;
		float fireDelay = 2.0f;
		
		auto minRotationIt = json.find("rotation_min");
		if (minRotationIt != json.end())
			minRotation = glm::radians(minRotationIt->get<float>());
		
		auto maxRotationIt = json.find("rotation_max");
		if (maxRotationIt != json.end())
			maxRotation = glm::radians(maxRotationIt->get<float>());
		
		auto rotationSpeedIt = json.find("rotation_speed");
		if (rotationSpeedIt != json.end())
			rotationSpeed = rotationSpeedIt->get<float>();
		
		auto fireDelayIt = json.find("fire_delay");
		if (fireDelayIt != json.end())
			fireDelay = fireDelayIt->get<float>();
		
		auto entity = std::make_unique<RocketTurret>(minRotation, maxRotation, rotationSpeed, fireDelay);
		
		return entity;
	}
}
