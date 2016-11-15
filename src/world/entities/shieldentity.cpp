#include "shieldentity.h"
#include "../gameworld.h"
#include "../../updateinfo.h"
#include "../../graphics/quadmesh.h"
#include "../../graphics/gl/shadermodule.h"
#include "../../graphics/spriterenderlist.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"

#include <glm/gtc/constants.hpp>

namespace TankGame
{
	StackObject<ShaderProgram> ShieldEntity::s_distortionShader;
	
	StackObject<Buffer> ShieldEntity::s_vertexBuffer;
	StackObject<VertexArray> ShieldEntity::s_vertexArray;
	GLsizei ShieldEntity::s_numVertices;
	
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
		
		s_vertexBuffer.Construct(vertices.size() * sizeof(Vertex), vertices.data(), 0);
		s_vertexArray.Construct();
		
		glEnableVertexArrayAttrib(s_vertexArray->GetID(), 0);
		glVertexArrayVertexBuffer(s_vertexArray->GetID(), 0, s_vertexBuffer->GetID(), 0, sizeof(Vertex));
		glVertexArrayAttribFormat(s_vertexArray->GetID(), 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(s_vertexArray->GetID(), 0, 0);
		
		glEnableVertexArrayAttrib(s_vertexArray->GetID(), 1);
		glVertexArrayVertexBuffer(s_vertexArray->GetID(), 1, s_vertexBuffer->GetID(), sizeof(float) * 2, sizeof(Vertex));
		glVertexArrayAttribFormat(s_vertexArray->GetID(), 1, 1, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(s_vertexArray->GetID(), 1, 1);
	}
	
	ShieldEntity::ShieldEntity()
	    : m_settingsBuffer(BufferAllocator::GetInstance().AllocateUnique(sizeof(float) * 13, GL_MAP_WRITE_BIT))
	{
		if (s_distortionShader.IsNull())
		{
			auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "shield.vs.glsl", GL_VERTEX_SHADER);
			auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "shield.fs.glsl", GL_FRAGMENT_SHADER);
			
			s_distortionShader.Construct<std::initializer_list<const ShaderModule*>>({ &vs, &fs });
			
			CallOnClose([] { s_distortionShader.Destroy(); });
		}
		
		if (s_vertexBuffer.IsNull())
			CreateVertexBuffer();
	}
	
	Circle ShieldEntity::GetBoundingCircle() const
	{
		return Circle(GetTransform().GetPosition(), m_radius * OUTER_VERTEX_DIST);
	}
	
	void ShieldEntity::Update(const UpdateInfo& updateInfo)
	{
		void* settingsMemory = glMapNamedBufferRange(*m_settingsBuffer, 0, sizeof(float) * 13,
		                                             GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		
		float rotation = 0;
		float sinR = std::sin(rotation);
		float cosR = std::cos(rotation) * m_radius;
		
		reinterpret_cast<float*>(settingsMemory)[0] = cosR;
		reinterpret_cast<float*>(settingsMemory)[1] = sinR;
		reinterpret_cast<float*>(settingsMemory)[4] = -sinR;
		reinterpret_cast<float*>(settingsMemory)[5] = cosR;
		reinterpret_cast<float*>(settingsMemory)[8] = GetTransform().GetPosition().x;
		reinterpret_cast<float*>(settingsMemory)[9] = GetTransform().GetPosition().y;
		reinterpret_cast<float*>(settingsMemory)[10] = m_radius;
		reinterpret_cast<float*>(settingsMemory)[11] = 0.0f;
		reinterpret_cast<float*>(settingsMemory)[12] = 0.0f;
		
		glUnmapNamedBuffer(*m_settingsBuffer);
	}
	
	void ShieldEntity::Draw(SpriteRenderList& spriteRenderList) const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, *m_settingsBuffer);
	}
	
	void ShieldEntity::DrawDistortions() const
	{
		s_distortionShader->Use();
		
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, *m_settingsBuffer);
		
		s_vertexArray->Bind();
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, s_numVertices);
	}
	
	void ShieldEntity::OnSpawned(GameWorld& gameWorld)
	{
		Entity::OnSpawned(gameWorld);
	}
}
