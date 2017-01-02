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
	std::unique_ptr<VertexArray> ShieldEntity::s_vertexArray;
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
			glm::vec2 m_position;
			float m_angle;
		};
		
		static_assert(sizeof(Vertex) == sizeof(float) * 3, "Invalid structure size.");
		
		std::array<Vertex, (DIVISIONS + 1) * 2> vertices;
		
		s_numVertices = vertices.size();
		
		for (int i = 0; i <= DIVISIONS; i++)
		{
			float angle = i * ANGLE;
			glm::vec2 vertexVec(std::cos(angle), std::sin(angle));
			
			vertices[i * 2].m_angle = angle;
			vertices[i * 2].m_position = vertexVec * CLOSE_VERTEX_DIST;
			
			vertices[i * 2 + 1].m_angle = angle;
			vertices[i * 2 + 1].m_position = vertexVec * OUTER_VERTEX_DIST;
		}
		
		s_vertexBuffer = std::make_unique<Buffer>(vertices.size() * sizeof(Vertex), vertices.data(), 0);
		s_vertexArray = std::make_unique<VertexArray>();
		
		glEnableVertexArrayAttrib(s_vertexArray->GetID(), 0);
		glVertexArrayVertexBuffer(s_vertexArray->GetID(), 0, s_vertexBuffer->GetID(), 0, sizeof(Vertex));
		glVertexArrayAttribFormat(s_vertexArray->GetID(), 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(s_vertexArray->GetID(), 0, 0);
		
		glEnableVertexArrayAttrib(s_vertexArray->GetID(), 1);
		glVertexArrayVertexBuffer(s_vertexArray->GetID(), 1, s_vertexBuffer->GetID(), sizeof(float) * 2, sizeof(Vertex));
		glVertexArrayAttribFormat(s_vertexArray->GetID(), 1, 1, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(s_vertexArray->GetID(), 1, 1);
	}
	
	ShieldEntity::ShieldEntity(float hp, int teamID, float radius)
	    : Hittable(hp, teamID),
	      m_settingsBuffer(BufferAllocator::GetInstance().AllocateUnique(sizeof(float) * 17, GL_MAP_WRITE_BIT)),
	      m_radius(radius)
	{
		if (s_distortionShader== nullptr)
		{
			auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "shield.vs.glsl", GL_VERTEX_SHADER);
			auto distFs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "shield-dist.fs.glsl", GL_FRAGMENT_SHADER);
			auto spriteFs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "shield-sprite.fs.glsl", GL_FRAGMENT_SHADER);
			
			s_distortionShader.reset(new ShaderProgram{ &vs, &distFs });
			s_spriteShader.reset(new ShaderProgram{ &vs, &spriteFs });
			
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
		
		void* settingsMemory = glMapNamedBuffer(*m_settingsBuffer, GL_WRITE_ONLY);
		
		float rotation = 0;
		float sinR = std::sin(rotation);
		float cosR = std::cos(rotation) * m_radius;
		
		glm::vec2 centerView = updateInfo.m_viewInfo.WorldToScreen(GetTransform().GetPosition());
		
		reinterpret_cast<float*>(settingsMemory)[0] = cosR;
		reinterpret_cast<float*>(settingsMemory)[1] = sinR;
		reinterpret_cast<float*>(settingsMemory)[4] = -sinR;
		reinterpret_cast<float*>(settingsMemory)[5] = cosR;
		reinterpret_cast<float*>(settingsMemory)[8] = GetTransform().GetPosition().x;
		reinterpret_cast<float*>(settingsMemory)[9] = GetTransform().GetPosition().y;
		reinterpret_cast<float*>(settingsMemory)[10] = centerView.x * 2.0f - 1.0f;
		reinterpret_cast<float*>(settingsMemory)[11] = centerView.y * 2.0f - 1.0f;
		reinterpret_cast<float*>(settingsMemory)[12] = m_radius;
		reinterpret_cast<float*>(settingsMemory)[13] = m_intensity;
		reinterpret_cast<float*>(settingsMemory)[14] = m_rippleAngle;
		reinterpret_cast<float*>(settingsMemory)[15] = m_rippleProgress;
		reinterpret_cast<float*>(settingsMemory)[16] = glm::pi<float>() / m_rippleProgress;
		
		glUnmapNamedBuffer(*m_settingsBuffer);
	}
	
	void ShieldEntity::DrawTranslucent(SpriteRenderList& spriteRenderList) const
	{
		s_spriteShader->Use();
		
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, *m_settingsBuffer);
		
		s_vertexArray->Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, s_numVertices);
	}
	
	void ShieldEntity::DrawDistortions() const
	{
		s_distortionShader->Use();
		
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, *m_settingsBuffer);
		
		s_vertexArray->Bind();
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
