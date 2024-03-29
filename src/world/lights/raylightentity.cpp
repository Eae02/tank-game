#include "raylightentity.h"
#include "../../graphics/gl/shadermodule.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"


namespace TankGame
{
	std::unique_ptr<ShaderProgram> RayLightEntity::s_shaderProgram;
	
	int RayLightEntity::s_positionUniformLocation;
	int RayLightEntity::s_directionUniformLocation;
	int RayLightEntity::s_heightUniformLocation;
	int RayLightEntity::s_worldTransformUniformLocation;
	
	RayLightEntity::RayLightEntity(glm::vec3 color, float intensity, Attenuation attenuation, float length, float height)
	    : m_uniformBuffer(BufferAllocator::GetInstance().AllocateUnique(sizeof(LightUniformBufferData), BufferUsage::DynamicUBO)),
	      m_flickerOffset(GenerateFlickerOffset()), m_height(height), m_length(length), m_color(color),
	      m_intensity(intensity), m_attenuation(attenuation)
	{
		m_uniformBufferOutOfDate = true;
		m_range = GetRange(m_color, m_intensity, m_attenuation);
	}
	
	Circle RayLightEntity::GetBoundingCircle() const
	{
		LightInfo lightInfo = *GetLightInfo();
		return Circle(lightInfo.m_position, lightInfo.m_range);
	}
	
	void RayLightEntity::SetColor(const glm::vec3& color)
	{
		if (m_color == color)
			return;
		m_color = color;
		m_uniformBufferOutOfDate = true;
		m_range = GetRange(m_color, m_intensity, m_attenuation);
	}
	
	void RayLightEntity::SetIntensity(float intensity)
	{
		if (FloatEqual(m_intensity, intensity))
			return;
		m_intensity = intensity;
		m_uniformBufferOutOfDate = true;
		m_range = GetRange(m_color, m_intensity, m_attenuation);
	}
	
	void RayLightEntity::SetAttenuation(const Attenuation& attenuation)
	{
		m_attenuation = attenuation;
		m_uniformBufferOutOfDate = true;
		m_range = GetRange(m_color, m_intensity, m_attenuation);
	}
	
	void RayLightEntity::SetFlickerIntensity(float flickerIntensity)
	{
		m_flickerIntensity = flickerIntensity;
		m_uniformBufferOutOfDate = true;
	}
	
	const ShaderProgram& RayLightEntity::GetShader() const
	{
		if (s_shaderProgram== nullptr)
		{
			s_shaderProgram = std::make_unique<ShaderProgram>(MakeShaderProgram("raylight.fs.glsl"));
			
			s_positionUniformLocation = s_shaderProgram->GetUniformLocation("position");
			s_directionUniformLocation = s_shaderProgram->GetUniformLocation("direction");
			s_heightUniformLocation = s_shaderProgram->GetUniformLocation("height");
			s_worldTransformUniformLocation = s_shaderProgram->GetUniformLocation("worldTransform");
		}
		
		return *s_shaderProgram;
	}
	
	void RayLightEntity::Bind() const
	{
		if (m_uniformBufferOutOfDate)
		{
			LightUniformBufferData data = {};
			data.colorTimesIntensity[0] = m_color.r * m_intensity;
			data.colorTimesIntensity[1] = m_color.g * m_intensity;
			data.colorTimesIntensity[2] = m_color.b * m_intensity;
			data.attenLin = m_attenuation.GetLinear();
			data.attenExp = m_attenuation.GetExponent();
			data.flickerIntensity = m_flickerIntensity;
			data.flickerOffset = m_flickerOffset;
			
			m_uniformBuffer->Update(0, sizeof(LightUniformBufferData), &data);
			
			m_uniformBufferOutOfDate = false;
		}
		
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uniformBuffer->GetID());
		
		glm::vec2 direction = GetTransform().GetForward() * m_length;
		glProgramUniform2f(s_shaderProgram->GetID(), s_directionUniformLocation, direction.x, direction.y);
		
		glm::vec2 pos2D = GetTransform().GetPosition();
		glProgramUniform2f(s_shaderProgram->GetID(), s_positionUniformLocation, pos2D.x, pos2D.y);
		
		glProgramUniform1f(s_shaderProgram->GetID(), s_heightUniformLocation, m_height);
		
		glm::mat3 worldTransform = GetWorldTransform();
		glProgramUniformMatrix3fv(s_shaderProgram->GetID(), s_worldTransformUniformLocation, 1, GL_FALSE,
		                          reinterpret_cast<GLfloat*>(&worldTransform));
	}
	
	std::optional<LightInfo> RayLightEntity::GetLightInfo() const
	{
		glm::vec2 forward = GetTransform().GetForward();
		float halfLen = m_length * 0.5f;
		
		return LightInfo(GetTransform().GetPosition() + forward * halfLen, m_range + halfLen);
	}
	
	glm::mat3 RayLightEntity::GetWorldTransform() const
	{
		float cosR = std::cos(GetTransform().GetRotation() - glm::half_pi<float>());
		float sinR = std::sin(GetTransform().GetRotation() - glm::half_pi<float>());
		
		float halfLen = m_length * 0.5f;
		
		glm::vec2 pos = GetTransform().GetPosition();
		
		return glm::mat3(
				1, 0, 0,
				0, 1, 0,
				pos.x, pos.y, 1
		) * glm::mat3(
				cosR, sinR, 0,
				-sinR,  cosR, 0,
				0, 0, 1
		) * glm::mat3(
				1, 0, 0,
				0, 1, 0,
				halfLen, 0, 1
		) * glm::mat3(
				m_range + halfLen, 0, 0,
				0, m_range, 0,
				0, 0, 1.0f
		);
	}
}
