#include "ilightsource.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"
#include "../../utils/memory/stackobject.h"
#include "../../graphics/gl/shadermodule.h"

#include <algorithm>

namespace TankGame
{
	static StackObject<ShaderModule> vertexShader;
	
	const ShaderModule& ILightSource::GetVertexShader()
	{
		if (vertexShader.IsNull())
		{
			vertexShader.Construct(ShaderModule::FromFile(GetResDirectory() / "shaders" / "lighting" / "light.vs.glsl",
			                                              GL_VERTEX_SHADER));
			
			CallOnClose([] { vertexShader.Destroy(); });
		}
		
		return *vertexShader;
	}
	
	float ILightSource::GetRange(glm::vec3 color, float intensity, Attenuation attenuation)
	{
		if (intensity < 1E-6)
			return 0;
		
		float maxChannel = std::max(color.x, std::max(color.y, color.z));
		
		float a = attenuation.GetExponent();
		float b = attenuation.GetLinear();
		float c = attenuation.GetExponent() - 8 * maxChannel * intensity;
		
		return (-b + std::sqrt(b * b - 4 * a * c)) / (2.0f * a);
	}
}
