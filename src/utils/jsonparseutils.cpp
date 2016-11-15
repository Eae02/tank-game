#include "jsonparseutils.h"
#include "utils.h"

#include <stdexcept>
#include <glm/gtc/color_space.hpp>

namespace TankGame
{
	glm::vec2 ParseVec2(const nlohmann::json& json)
	{
		if (json.is_number())
			return glm::vec2(json.get<float>());
		if (json.is_array() && json.size() == 2)
			return { json[0].get<float>(), json[1].get<float>() };
		throw std::runtime_error("Invalid vec2 in json.");
	}
	
	void ParseTransform(const nlohmann::json& json, Transform& transform)
	{
		auto positionIt = json.find("position");
		if (positionIt != json.end())
			transform.SetPosition(ParseVec2(*positionIt));
		
		auto scaleIt = json.find("scale");
		if (scaleIt != json.end())
			transform.SetScale(ParseVec2(*scaleIt));
		
		auto rotationIt = json.find("rotation");
		if (rotationIt != json.end())
			transform.SetRotation(glm::radians(rotationIt->get<float>()));
		
		auto rotationCenterIt = json.find("rotation_center");
		if (rotationCenterIt != json.end())
			transform.SetCenterOfRotation(ParseVec2(*rotationCenterIt));
	}
	
	glm::vec3 ParseColor(const nlohmann::json& json)
	{
		if (json.is_string())
		{
			int hexColor = std::stoi(json.get<std::string>(), nullptr, 16);
			return ParseColorHexCodeSRGB(hexColor);
		}
		
		if (json.is_object())
		{
			glm::vec3 colorSrgb(json["r"].get<float>(), json["g"].get<float>(), json["b"].get<float>());
			return glm::convertSRGBToLinear(colorSrgb);
		}
		
		throw std::runtime_error("Invalid color in json.");
	}
}
