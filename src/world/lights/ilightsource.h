#pragma once

#include "attenuation.h"
#include "../../utils/abstract.h"

#include <glm/glm.hpp>

namespace TankGame
{
	struct LightInfo
	{
		glm::vec2 m_position;
		float m_range;
		
		LightInfo() = default;
		
		inline LightInfo(const glm::vec2& position, float range)
		    : m_position(position), m_range(range) { }
	};
	
	class ILightSource : public Abstract
	{
	public:
		virtual const class ShaderProgram& GetShader() const = 0;
		virtual void Bind() const = 0;
		
		virtual LightInfo GetLightInfo() const = 0;
		
		static const class ShaderModule& GetVertexShader();
		
		static float GetRange(glm::vec3 color, float intensity, Attenuation attenuation);
		
		static float GenerateFlickerOffset();
	};
}
