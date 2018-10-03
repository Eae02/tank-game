#include "ilightsource.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"
#include "../../graphics/gl/shadermodule.h"

#include <memory>
#include <algorithm>
#include <random>
#include <glm/gtc/constants.hpp>

namespace TankGame
{
	static std::unique_ptr<ShaderModule> vertexShader;
	
	const ShaderModule& ILightSource::GetVertexShader()
	{
		if (vertexShader== nullptr)
		{
			const fs::path vsPath = GetResDirectory() / "shaders" / "lighting" / "light.vs.glsl";
			vertexShader = std::make_unique<ShaderModule>(ShaderModule::FromFile(vsPath, GL_VERTEX_SHADER));
			
			CallOnClose([] { vertexShader = nullptr; });
		}
		
		return *vertexShader;
	}
	
	float ILightSource::GetRange(glm::vec3 color, float intensity, Attenuation attenuation)
	{
		float maxChannel = std::max(color.x, std::max(color.y, color.z));
		
		float a = attenuation.GetExponent();
		float b = attenuation.GetLinear();
		float c = std::min(attenuation.GetExponent() - 8 * maxChannel * intensity, 0.0f);
		
		return std::max((-b + std::sqrt(b * b - 4 * a * c)) / (2.0f * a), 0.1f);
	}
	
	static std::uniform_real_distribution<float> flickerOffsetGen(0.0f, glm::two_pi<float>());
	
	float ILightSource::GenerateFlickerOffset()
	{
		return flickerOffsetGen(randomGen);
	}
}
