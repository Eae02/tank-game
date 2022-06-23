#include "shieldentity.h"
#include "../gameworld.h"
#include "../../updateinfo.h"
#include "../../graphics/gl/specializationinfo.h"
#include "../../graphics/quadmesh.h"
#include "../../graphics/gl/shadermodule.h"
#include "../../graphics/spriterenderlist.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"

#include <glm/gtc/constants.hpp>

namespace TankGame
{
	std::unique_ptr<ShaderProgram> ShieldEntity::s_distortionShader;
	std::unique_ptr<ShaderProgram> ShieldEntity::s_spriteShader;
	
	std::unique_ptr<Buffer> ShieldEntity::s_vertexBuffer;
	std::unique_ptr<VertexInputState> ShieldEntity::s_vertexInputState;
	GLsizei ShieldEntity::s_numVertices;
	
	constexpr float ShieldEntity::RIPPLE_TIME;
	
	constexpr int DIVISIONS = 16;
	const float INTENSITY = 20;
	const float ANGLE = glm::two_pi<float>() / DIVISIONS;
	
	const float RADIUS = glm::pi<float>() / INTENSITY;
	const float CLOSE_VERTEX_DIST = 1.0f - RADIUS;
	const float OUTER_VERTEX_DIST = 1.0f + RADIUS / std::cos(ANGLE / 2.0f);
	
	void ShieldEntity::CreateVertexBuffer()
	{
		struct Vertex
		{
			float x;
			float y;
			float angle;
		};
		
		static_assert(sizeof(Vertex) == sizeof(float) * 3, "Invalid structure size.");
		
		std::array<Vertex, (DIVISIONS + 1) * 2> vertices;
		
		s_numVertices = vertices.size();
		
		for (int i = 0; i <= DIVISIONS; i++)
		{
			float angle = i * ANGLE;
			glm::vec2 vertexVec(std::cos(angle), std::sin(angle));
			
			vertices[i * 2].angle = angle;
			vertices[i * 2].x = vertexVec.x * CLOSE_VERTEX_DIST;
			vertices[i * 2].y = vertexVec.y * CLOSE_VERTEX_DIST;
			
			vertices[i * 2 + 1].angle = angle;
			vertices[i * 2 + 1].x = vertexVec.x * OUTER_VERTEX_DIST;
			vertices[i * 2 + 1].y = vertexVec.y * OUTER_VERTEX_DIST;
		}
		
		s_vertexBuffer = std::make_unique<Buffer>(vertices.size() * sizeof(Vertex), vertices.data(), BufferUsage::StaticVertex);
		s_vertexInputState = std::make_unique<VertexInputState>();
		
		s_vertexInputState->UpdateAttribute(0, s_vertexBuffer->GetID(), VertexAttribFormat::Float32_3, 0, sizeof(Vertex));
	}
	
	struct SettingsBufferData
	{
		glm::vec2 transformRS;
		glm::vec2 centerWorld;
		glm::vec2 centerView;
		float radius;
		float intensity;
		float rippleCenter;
		float rippleDistance;
		float rippleIntensity;
	};
	
	ShieldEntity::ShieldEntity(float hp, int teamID, float radius)
	    : Hittable(hp, teamID),
	      m_settingsBuffer(BufferAllocator::GetInstance().AllocateUnique(sizeof(SettingsBufferData), BufferUsage::MapWritePersistentUBO_MultiFrame)),
	      m_radius(radius)
	{
		if (s_distortionShader== nullptr)
		{
			auto vs = ShaderModule::FromFile(resDirectoryPath / "shaders" / "shield.vs.glsl", GL_VERTEX_SHADER);
			auto distFs = ShaderModule::FromFile(resDirectoryPath / "shaders" / "shield-dist.fs.glsl", GL_FRAGMENT_SHADER);
			auto spriteFs = ShaderModule::FromFile(resDirectoryPath / "shaders" / "shield-sprite.fs.glsl", GL_FRAGMENT_SHADER);
			
			s_distortionShader = std::make_unique<ShaderProgram>(vs, distFs);
			s_spriteShader = std::make_unique<ShaderProgram>(vs, spriteFs);
			
			glm::vec3 color = ParseColorHexCodeSRGB(0x90C3D4) * 3.0f;
			glProgramUniform3fv(s_spriteShader->GetID(), s_spriteShader->GetUniformLocation("color"), 1,
			                    reinterpret_cast<GLfloat*>(&color));
			
			CallOnClose([] { s_distortionShader = nullptr; s_spriteShader = nullptr; });
		}
		
		if (s_vertexBuffer== nullptr)
			CreateVertexBuffer();
		
		SetEditorVisible(false);
	}
	
	Circle ShieldEntity::GetBoundingCircle() const
	{
		return Circle(GetTransform().GetPosition(), m_radius * OUTER_VERTEX_DIST);
	}
	
	Circle ShieldEntity::GetHitCircle() const
	{
		return Circle(GetTransform().GetPosition(), m_radius);
	}
	
	void ShieldEntity::Update(const UpdateInfo& updateInfo)
	{
		m_rippleProgress += updateInfo.m_dt * 12;
		m_intensity = glm::min(m_intensity + updateInfo.m_dt * 2, 1.0f);
		
		float rotation = 0;
		float sinR = std::sin(rotation);
		float cosR = std::cos(rotation) * m_radius;
		
		glm::vec2 centerView = updateInfo.m_viewInfo.WorldToScreen(GetTransform().GetPosition());
		
		auto& settingsData = *reinterpret_cast<SettingsBufferData*>(m_settingsBuffer->CurrentFrameMappedMemory());
		
		settingsData.transformRS = glm::vec2(cosR, sinR);
		settingsData.centerWorld = GetTransform().GetPosition();
		settingsData.centerView = centerView * 2.0f - 1.0f;
		settingsData.radius = m_radius;
		settingsData.intensity = m_intensity;
		settingsData.rippleCenter = m_rippleAngle;
		settingsData.rippleDistance = m_rippleProgress;
		settingsData.rippleIntensity = glm::pi<float>() / m_rippleProgress;
		
		m_settingsBuffer->FlushCurrentFrameMappedMemory();
	}
	
	void ShieldEntity::DrawTranslucent(SpriteRenderList& spriteRenderList) const
	{
		s_spriteShader->Use();
		
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_settingsBuffer->GetID(),
		                  m_settingsBuffer->CurrentFrameOffset(), sizeof(SettingsBufferData));
		
		s_vertexInputState->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, s_numVertices);
	}
	
	void ShieldEntity::DrawDistortions() const
	{
		s_distortionShader->Use();
		
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_settingsBuffer->GetID(),
		                  m_settingsBuffer->CurrentFrameOffset(), sizeof(SettingsBufferData));
		
		s_vertexInputState->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, s_numVertices);
	}
	
	void ShieldEntity::Ripple(float originAngle)
	{
		m_rippleAngle = originAngle;
		m_rippleProgress = 1E-6f;
	}
	
	void ShieldEntity::OnKilled()
	{
		Despawn();
	}
}
