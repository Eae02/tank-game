#include "tileshadowcastersbuffer.h"

#include "../world/tilegrid.h"
#include "../utils/ioutils.h"
#include "../utils/utils.h"
#include "gl/shadermodule.h"
#include "shadowrenderer.h"
#include "tilegridmaterial.h"

namespace TankGame
{
	std::unique_ptr<ShaderProgram> TileShadowCastersBuffer::s_shadowShader;
	
	void TileShadowCastersBuffer::BindShadowShader()
	{
		if (s_shadowShader == nullptr)
		{
			auto vs = ShaderModule::FromFile(
				GetResDirectory() / "shaders" / "lighting" / "shadows" / "tileshadow.vs.glsl", GL_VERTEX_SHADER);
			
			s_shadowShader.reset(new ShaderProgram{ &vs, &ShadowRenderer::GetFragmentShader() });
			
			CallOnClose([] { s_shadowShader = nullptr; });
		}
		
		s_shadowShader->Use();
	}
	
	struct ShadowCaster
	{
		ShadowCaster(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 normal)
			: m_position1(pos1), m_normal1(normal), m_position2(pos2), m_normal2(normal) { }
		
		glm::vec2 m_position1;
		glm::vec2 m_normal1;
		glm::vec2 m_position2;
		glm::vec2 m_normal2;
	};
	
	TileShadowCastersBuffer::Data TileShadowCastersBuffer::BuildBuffers(
		const TileGrid& tileGrid, const TileGridMaterial& material)
	{
		std::vector<glm::vec4> vertices;
		std::vector<uint32_t> indices;
		
		static const glm::ivec2 faceNormals[] = { { 0, -1 }, { 0, 1 }, { -1, 0 }, { 1, 0 } };
		
		for (int y = 0; y < tileGrid.GetHeight(); y++)
		{
			for (int x = 0; x < tileGrid.GetWidth(); x++)
			{
				uint8_t tileID = tileGrid.GetTileID({ x, y });
				if (!material.IsSolid(tileID))
					continue;
				
				for (int f = 0; f < 4; f++)
				{
					glm::ivec2 forwardTilePos = glm::ivec2(x, y) + faceNormals[f];
					if (forwardTilePos.x < 0 || forwardTilePos.y < 0 || forwardTilePos.x >= tileGrid.GetWidth()
					    || forwardTilePos.y >= tileGrid.GetHeight())
					{
						continue;
					}
					
					if (material.IsSolid(tileGrid.GetTileID(forwardTilePos)))
						continue;
					
					glm::vec2 left(faceNormals[f].y, -faceNormals[f].x);
					glm::vec2 centerLine = glm::vec2(x + 0.5f, y + 0.5f) + glm::vec2(faceNormals[f]) * 0.5f;
					
					glm::vec2 v1 = centerLine + left * 0.5f;
					glm::vec2 v2 = centerLine - left * 0.5f;
					
					for (int relIndex : { 0, 1, 2, 2, 1, 3 })
						indices.push_back(vertices.size() + relIndex);
					
					vertices.emplace_back(v1, faceNormals[f]);
					vertices.emplace_back(v2, faceNormals[f]);
					vertices.emplace_back(v1, faceNormals[f]);
					vertices.emplace_back(v2, faceNormals[f]);
				}
			}
		}
		
		return Data {
			(GLuint)indices.size(),
			Buffer(vertices.size() * sizeof(glm::vec4), vertices.data(), BufferUsage::StaticData),
			Buffer(indices.size() * sizeof(uint32_t), indices.data(), BufferUsage::StaticData)
		};
	}
	
	TileShadowCastersBuffer::TileShadowCastersBuffer(const TileGrid& tileGrid, const TileGridMaterial& material)
		: m_data(BuildBuffers(tileGrid, material))
	{
		glEnableVertexArrayAttrib(m_vertexArray.GetID(), 0);
		glVertexArrayVertexBuffer(m_vertexArray.GetID(), 0, m_data.vertexBuffer.GetID(), 0, sizeof(glm::vec2) * 2);
		glVertexArrayAttribFormat(m_vertexArray.GetID(), 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(m_vertexArray.GetID(), 0, 0);
		
		glEnableVertexArrayAttrib(m_vertexArray.GetID(), 1);
		glVertexArrayVertexBuffer(m_vertexArray.GetID(), 1, m_data.vertexBuffer.GetID(), sizeof(glm::vec2), sizeof(glm::vec2) * 2);
		glVertexArrayAttribFormat(m_vertexArray.GetID(), 1, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(m_vertexArray.GetID(), 1, 1);
		
		glVertexArrayElementBuffer(m_vertexArray.GetID(), m_data.indexBuffer.GetID());
	}
	
	void TileShadowCastersBuffer::Draw() const
	{
		BindShadowShader();
		
		m_vertexArray.Bind();
		
		glDrawElements(GL_TRIANGLES, m_data.numIndices, GL_UNSIGNED_INT, nullptr);
	}
}
