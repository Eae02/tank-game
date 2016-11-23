#include "tileshadowcastersbuffer.h"

#include "../world/tilegrid.h"
#include "../utils/ioutils.h"
#include "../utils/utils.h"
#include "gl/shadermodule.h"
#include "shadowrenderer.h"
#include "tilegridmaterial.h"

namespace TankGame
{
	StackObject<ShaderProgram> TileShadowCastersBuffer::s_shadowShader;
	
	void TileShadowCastersBuffer::BindShadowShader()
	{
		if (s_shadowShader.IsNull())
		{
			auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "lighting" / "shadows" / "tileshadow.vs.glsl",
			                                 GL_VERTEX_SHADER);
			
			s_shadowShader.Construct<std::initializer_list<const ShaderModule*>>(
				{ &vs, &ShadowRenderer::GetGeometryShader(), &ShadowRenderer::GetFragmentShader() });
			
			CallOnClose([] { s_shadowShader.Destroy(); });
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
	
	static Buffer BuildBuffer(const TileGrid& tileGrid, const TileGridMaterial& material, GLuint& vertexCountOut)
	{
		std::vector<ShadowCaster> shadowCasters;
		
		glm::ivec2 faceNormals[] = { { 0, -1 }, { 0, 1 }, { -1, 0 }, { 1, 0 } };
		
		vertexCountOut = 0;
		
		for (int y = 0; y < tileGrid.GetHeight(); y++)
		{
			for (int x = 0; x < tileGrid.GetWidth(); x++)
			{
				uint8_t tileID = tileGrid.GetTileID(x, y);
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
					
					if (material.IsSolid(tileGrid.GetTileID(forwardTilePos.x, forwardTilePos.y)))
						continue;
					
					glm::vec2 left(faceNormals[f].y, -faceNormals[f].x);
					glm::vec2 centerLine = glm::vec2(x + 0.5f, y + 0.5f) + glm::vec2(faceNormals[f]) * 0.5f;
					
					shadowCasters.emplace_back(centerLine + left * 0.5f, centerLine - left * 0.5f, faceNormals[f]);
					
					vertexCountOut += 2;
				}
			}
		}
		
		return Buffer(shadowCasters.size() * sizeof(ShadowCaster), shadowCasters.data(), 0);
	}
	
	TileShadowCastersBuffer::TileShadowCastersBuffer(const TileGrid& tileGrid, const TileGridMaterial& material)
		: m_buffer(BuildBuffer(tileGrid, material, m_numVertices))
	{
		glEnableVertexArrayAttrib(m_vertexArray.GetID(), 0);
		glVertexArrayVertexBuffer(m_vertexArray.GetID(), 0, m_buffer.GetID(), 0, sizeof(glm::vec2) * 2);
		glVertexArrayAttribFormat(m_vertexArray.GetID(), 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(m_vertexArray.GetID(), 0, 0);
		
		glEnableVertexArrayAttrib(m_vertexArray.GetID(), 1);
		glVertexArrayVertexBuffer(m_vertexArray.GetID(), 1, m_buffer.GetID(), sizeof(glm::vec2), sizeof(glm::vec2) * 2);
		glVertexArrayAttribFormat(m_vertexArray.GetID(), 1, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(m_vertexArray.GetID(), 1, 1);
	}
	
	void TileShadowCastersBuffer::Draw() const
	{
		BindShadowShader();
		
		m_vertexArray.Bind();
		
		glDrawArrays(GL_LINES, 0, m_numVertices);
	}
}
