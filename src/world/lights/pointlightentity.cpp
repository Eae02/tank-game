#include "pointlightentity.h"
#include "../gameworld.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"
#include "../../utils/mathutils.h"
#include "../../graphics/gl/shadermodule.h"
#include "../../updateinfo.h"

#include <glm/gtc/color_space.hpp>
#include <imgui.h>
#include <random>

namespace TankGame
{
	std::unique_ptr<ShaderProgram> PointLightEntity::s_shaderProgram;
	int PointLightEntity::s_positionUniformLocation;
	int PointLightEntity::s_worldTransformUniformLocation;
	
	const ShaderProgram& PointLightEntity::GetShader() const
	{
		if (s_shaderProgram== nullptr)
		{
			s_shaderProgram = std::make_unique<ShaderProgram>(MakeShaderProgram("pointlight.fs.glsl"));
			
			s_positionUniformLocation = s_shaderProgram->GetUniformLocation("position");
			s_worldTransformUniformLocation = s_shaderProgram->GetUniformLocation("worldTransform");
			
			CallOnClose([] { s_shaderProgram = nullptr; });
		}
		
		return *s_shaderProgram;
	}
	
	const char* PointLightEntity::GetSerializeClassName() const
	{ return "PointLight"; }
	
	const char* PointLightEntity::GetObjectName() const
	{ return "Point Light"; }
	
	void PointLightEntity::RenderProperties()
	{
		RenderBaseProperties(Transform::Properties::Position);
		RenderLightSourceProperties();
	}
	
	std::unique_ptr<Entity> PointLightEntity::Clone() const
	{
		auto clone = std::make_unique<PointLightEntity>(GetColor(), GetIntensity(), GetAttenuation(), GetHeight());
		clone->SetFlickers(Flickers());
		clone->GetTransform() = GetTransform();
		clone->SetShadowMode(GetShadowMode());
		return clone;
	}
}
