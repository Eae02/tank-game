#include "spotlightentity.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"
#include "../../utils/mathutils.h"
#include "../../graphics/gl/shadermodule.h"

#include <imgui.h>

namespace TankGame
{
	StackObject<ShaderProgram> SpotLightEntity::s_shaderProgram;
	int SpotLightEntity::s_positionUniformLocation;
	int SpotLightEntity::s_directionUniformLocation;
	int SpotLightEntity::s_worldTransformUniformLocation;
	
	SpotLightEntity::SpotLightEntity(glm::vec3 color, float intensity, float cutoffAngle,
	                                 Attenuation attenuation, float height)
	    : LightSourceEntity(color, intensity, attenuation, height, sizeof(float) * 9)
	{
		SetCutoffAngle(cutoffAngle);
	}
	
	const ShaderProgram& SpotLightEntity::GetShader() const
	{
		if (s_shaderProgram.IsNull())
		{
			ShaderModule fragmentShader = ShaderModule::FromFile(
					GetResDirectory() / "shaders" / "lighting" / "spotlight.fs.glsl", GL_FRAGMENT_SHADER);
			
			s_shaderProgram .Construct(ShaderProgram{ &GetVertexShader(), &fragmentShader });
			
			s_positionUniformLocation = s_shaderProgram->GetUniformLocation("position");
			s_directionUniformLocation = s_shaderProgram->GetUniformLocation("direction");
			s_worldTransformUniformLocation = s_shaderProgram->GetUniformLocation("worldTransform");
			
			CallOnClose([] { s_shaderProgram.Destroy(); });
		}
		
		return *s_shaderProgram;
	}
	
	void SpotLightEntity::Bind() const
	{
		LightSourceEntity::Bind();
		
		glm::vec2 forward = GetTransform().GetForward();
		glProgramUniform2f(s_shaderProgram->GetID(), s_directionUniformLocation, forward.x, forward.y);
	}
	
	void SpotLightEntity::UpdateUniformBuffer(void* memory) const
	{
		LightSourceEntity::UpdateUniformBuffer(memory);
		reinterpret_cast<float*>(memory)[8] = m_cutoff;
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
		RenderTransformProperty(Transform::Properties::Position | Transform::Properties::Rotation);
		
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
