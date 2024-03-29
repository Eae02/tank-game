#include "spotlightentity.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"
#include "../../utils/mathutils.h"
#include "../../graphics/gl/shadermodule.h"

#include <imgui.h>

namespace TankGame
{
	std::unique_ptr<ShaderProgram> SpotLightEntity::s_shaderProgram;
	int SpotLightEntity::s_positionUniformLocation;
	int SpotLightEntity::s_directionUniformLocation;
	int SpotLightEntity::s_worldTransformUniformLocation;
	
	const ShaderProgram& SpotLightEntity::GetShader() const
	{
		if (s_shaderProgram == nullptr)
		{
			s_shaderProgram = std::make_unique<ShaderProgram>(MakeShaderProgram("spotlight.fs.glsl"));
			
			s_positionUniformLocation = s_shaderProgram->GetUniformLocation("position");
			s_directionUniformLocation = s_shaderProgram->GetUniformLocation("direction");
			s_worldTransformUniformLocation = s_shaderProgram->GetUniformLocation("worldTransform");
			
			CallOnClose([] { s_shaderProgram = nullptr; });
		}
		
		return *s_shaderProgram;
	}
	
	void SpotLightEntity::Bind() const
	{
		LightSourceEntity::Bind();
		
		glm::vec2 forward = GetTransform().GetForward();
		glProgramUniform2f(s_shaderProgram->GetID(), s_directionUniformLocation, forward.x, forward.y);
	}
	
	float SpotLightEntity::GetExtraUniformValue() const
	{
		return m_cutoff;
	}
	
	void SpotLightEntity::SetCutoffAngle(float cutoffAngle)
	{
		m_cutoffAngle = cutoffAngle;
		m_cutoff = std::cos(cutoffAngle);
		InvalidateUniformBuffer();
	}
	
	const char* SpotLightEntity::GetObjectName() const
	{
		return "Spot Light";
	}
	
	void SpotLightEntity::RenderProperties()
	{
		RenderBaseProperties(Transform::Properties::Position | Transform::Properties::Rotation);
		
		RenderLightSourceProperties();
		
		if (ImGui::SliderAngle("Cutoff", &m_cutoffAngle, 0, 90))
			SetCutoffAngle(m_cutoffAngle);
	}
	
	std::unique_ptr<Entity> SpotLightEntity::Clone() const
	{
		auto clone = std::make_unique<SpotLightEntity>(GetColor(), GetIntensity(), m_cutoffAngle, GetAttenuation(), GetHeight());
		clone->GetTransform() = GetTransform();
		clone->SetShadowMode(GetShadowMode());
		return clone;
	}
}
