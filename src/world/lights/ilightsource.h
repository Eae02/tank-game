#pragma once

#include "attenuation.h"
#include "../../utils/abstract.h"
#include "../../graphics/gl/shaderprogram.h"

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
	
	enum class EntityShadowModes
	{
		None = 0,
		Static = 1,
		Dynamic = 2
	};
	
	class ILightSource : public Abstract
	{
	public:
		virtual const ShaderProgram& GetShader() const = 0;
		virtual void Bind() const = 0;
		
		virtual std::optional<LightInfo> GetLightInfo() const = 0;
		
		virtual class ShadowMap* GetShadowMap() const = 0;
		virtual void InvalidateShadowMap() = 0;
		
		static const ShaderModule& GetVertexShader();
		static ShaderProgram MakeShaderProgram(const std::string& fragmentShaderName);
		
		static float GetRange(glm::vec3 color, float intensity, Attenuation attenuation);
		
		static float GenerateFlickerOffset();
		
		static constexpr int NORMALS_SPECULAR_TEXTURE_BINDING = 0;
		static constexpr int SHADOW_MAP_TEXTURE_BINDING = 1;
	};
	
	struct LightUniformBufferData
	{
		float colorTimesIntensity[3];
		float extra;
		float attenLin;
		float attenExp;
		float flickerIntensity;
		float flickerOffset;
	};
}
