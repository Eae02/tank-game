#include "pointlightentity.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"
#include "../../utils/mathutils.h"
#include "../../graphics/gl/shadermodule.h"

#include <glm/gtc/color_space.hpp>
#include <imgui.h>

namespace TankGame
{
	StackObject<ShaderProgram> PointLightEntity::s_shaderProgram;
	int PointLightEntity::s_positionUniformLocation;
	int PointLightEntity::s_worldTransformUniformLocation;
	
	PointLightEntity::PointLightEntity(glm::vec3 color, float intensity, Attenuation attenuation, float height)
	    : LightSourceEntity(color, intensity, attenuation, height, 6 * sizeof(float)) { }
	
	const ShaderProgram& PointLightEntity::GetShader() const
	{
		if (s_shaderProgram.IsNull())
		{
			ShaderModule fragmentShader = ShaderModule::FromFile(
					GetResDirectory() / "shaders" / "lighting" / "pointlight.fs.glsl", GL_FRAGMENT_SHADER);
			
			s_shaderProgram .Construct(ShaderProgram{ &GetVertexShader(), &fragmentShader });
			
			s_positionUniformLocation = s_shaderProgram->GetUniformLocation("position");
			s_worldTransformUniformLocation = s_shaderProgram->GetUniformLocation("worldTransform");
			
			CallOnClose([] { s_shaderProgram.Destroy(); });
		}
		
		return *s_shaderProgram;
	}
	
	const char* PointLightEntity::GetSerializeClassName() const
	{ return "PointLight"; }
	
	const char* PointLightEntity::GetObjectName() const
	{ return "Point Light"; }
	
	void PointLightEntity::RenderProperties()
	{
		RenderTransformProperty(Transform::Properties::Position);
		RenderLightSourceProperties();
	}
	
	std::unique_ptr<Entity> PointLightEntity::Clone() const
	{
		auto clone = std::make_unique<PointLightEntity>(GetColor(), GetIntensity(), GetAttenuation(), GetHeight());
		clone->GetTransform() = GetTransform();
		clone->SetShadowMode(GetShadowMode());
		return clone;
	}
}
