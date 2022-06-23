#include "tileshadowcastersbuffer.h"
#include "gl/shadermodule.h"
#include "shadowrenderer.h"
#include "shadowmap.h"
#include "gl/specializationinfo.h"
#include "tilegridmaterial.h"
#include "../world/tilegrid.h"
#include "../utils/ioutils.h"
#include "../utils/utils.h"
#include "../world/lights/ilightsource.h"

#include <unordered_map>

namespace TankGame
{
	std::unique_ptr<ShaderProgram> TileShadowCastersBuffer::s_shadowShader;
	
	void TileShadowCastersBuffer::BindShadowShader()
	{
		if (s_shadowShader == nullptr)
		{
			SpecializationInfo specInfo;
			
			std::unique_ptr<ShaderModule> fs;
			if (!ShadowMap::useDepthShadowMaps)
			{
				fs = std::make_unique<ShaderModule>(ShaderModule::FromResFile("lighting/shadows/shadow.fs.glsl"));
				specInfo.SetConstant("HAS_FRAGMENT_SHADER", "1");
			}
			
			s_shadowShader = std::make_unique<ShaderProgram>(
				ShaderModule::FromResFile("lighting/shadows/tileshadow.vs.glsl", &specInfo), fs.get());
			
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
	
	struct __attribute__((packed)) Vertex
	{
		float x;
		float y;
		int8_t pairOffsetX;
		int8_t pairOffsetY;
		int8_t project;
		int8_t _padding;
	};
	
	TileShadowCastersBuffer::Data TileShadowCastersBuffer::BuildBuffers(
		const TileGrid& tileGrid, const TileGridMaterial& material)
	{
		uint32_t numRegionsX = (tileGrid.GetWidth() + REGION_SIZE - 1) / REGION_SIZE;
		uint32_t numRegionsY = (tileGrid.GetHeight() + REGION_SIZE - 1) / REGION_SIZE;
		std::unique_ptr<RegionRange[]> regionRanges = std::make_unique<RegionRange[]>(numRegionsX * numRegionsY);
		
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		
		std::unordered_map<glm::ivec2, uint32_t, IVec2Hash> pushedVerticesMap;
		
		auto PushVertexPair = [&] (glm::ivec2 pos) -> uint32_t
		{
			auto it = pushedVerticesMap.find(pos);
			if (it != pushedVerticesMap.end())
				return it->second;
			uint32_t idx = vertices.size();
			pushedVerticesMap.emplace(pos, idx);
			vertices.push_back(Vertex { .x=static_cast<float>(pos.x), .y=static_cast<float>(pos.y), .project=0 });
			vertices.push_back(Vertex { .x=static_cast<float>(pos.x), .y=static_cast<float>(pos.y), .project=1 });
			return idx;
		};
		
		auto PushTriangle = [&] (uint32_t startLoVertex, uint32_t endLoVertex)
		{
			uint32_t helperVertexIdx = vertices.size();
			Vertex& helperVertex = vertices.emplace_back();
			helperVertex.x = vertices[startLoVertex].x;
			helperVertex.y = vertices[startLoVertex].y;
			helperVertex.pairOffsetX = static_cast<int8_t>(vertices[endLoVertex].x - vertices[startLoVertex].x);
			helperVertex.pairOffsetY = static_cast<int8_t>(vertices[endLoVertex].y - vertices[startLoVertex].y);
			helperVertex.project = 1;
			
			indices.push_back(startLoVertex);
			indices.push_back(startLoVertex + 1);
			indices.push_back(helperVertexIdx);
			
			indices.push_back(startLoVertex);
			indices.push_back(helperVertexIdx);
			indices.push_back(endLoVertex);
			
			indices.push_back(endLoVertex);
			indices.push_back(helperVertexIdx);
			indices.push_back(endLoVertex + 1);
		};
		
		auto IsShadowCaster = [&] (int x, int y)
		{
			glm::ivec2 pos(x, y);
			return !tileGrid.InRange(pos) || material.IsSolid(tileGrid.GetTileID(pos));
		};
		
		for (uint32_t ry = 0; ry < numRegionsY; ry++)
		{
			int regMaxY = std::min<int>((ry + 1) * REGION_SIZE, tileGrid.GetHeight());
			for (uint32_t rx = 0; rx < numRegionsX; rx++)
			{
				RegionRange& regionRange = regionRanges[rx + ry * numRegionsX];
				regionRange.firstIndex = indices.size();
				
				int regMaxX = std::min<int>((rx + 1) * REGION_SIZE, tileGrid.GetWidth());
				
				//Edges parallel to the x axis
				for (int y = ry * REGION_SIZE; y < regMaxY; y++)
				{
					bool hasActiveEdge = false;
					uint32_t edgeStartLoVertex;
					for (int x = rx * REGION_SIZE; x <= regMaxX; x++)
					{
						bool edge = x != regMaxX && (IsShadowCaster(x, y) != IsShadowCaster(x, y - 1));
						if (edge && !hasActiveEdge)
						{
							edgeStartLoVertex = PushVertexPair(glm::ivec2(x, y));
						}
						else if (!edge && hasActiveEdge)
						{
							uint32_t edgeEndLoVertex = PushVertexPair(glm::ivec2(x, y));
							PushTriangle(edgeStartLoVertex, edgeEndLoVertex);
						}
						hasActiveEdge = edge;
					}
				}
				
				//Edges parallel to the y axis
				for (int x = rx * REGION_SIZE; x < regMaxX; x++)
				{
					bool hasActiveEdge = false;
					uint32_t edgeStartLoVertex;
					for (int y = ry * REGION_SIZE; y <= regMaxY; y++)
					{
						bool edge = y != regMaxY && (IsShadowCaster(x, y) != IsShadowCaster(x - 1, y));
						if (edge && !hasActiveEdge)
						{
							edgeStartLoVertex = PushVertexPair(glm::ivec2(x, y));
						}
						else if (!edge && hasActiveEdge)
						{
							uint32_t edgeEndLoVertex = PushVertexPair(glm::ivec2(x, y));
							PushTriangle(edgeStartLoVertex, edgeEndLoVertex);
						}
						hasActiveEdge = edge;
					}
				}
				
				regionRange.lastIndex = indices.size();
			}
		}
		
		return Data {
			.regionRanges = std::move(regionRanges),
			.numRegionsX  = numRegionsX,
			.numRegionsY  = numRegionsY,
			.vertexBuffer = Buffer(vertices.size() * sizeof(Vertex), vertices.data(), BufferUsage::StaticVertex),
			.indexBuffer  = Buffer(indices.size() * sizeof(uint32_t), indices.data(), BufferUsage::StaticIndex)
		};
	}
	
	TileShadowCastersBuffer::TileShadowCastersBuffer(const TileGrid& tileGrid, const TileGridMaterial& material)
		: m_data(BuildBuffers(tileGrid, material))
	{
		m_vertexInputState.UpdateAttribute(0, m_data.vertexBuffer.GetID(), VertexAttribFormat::Float32_2, 0, sizeof(Vertex));
		m_vertexInputState.UpdateAttribute(1, m_data.vertexBuffer.GetID(), VertexAttribFormat::UInt8_3, 8, sizeof(Vertex));
	}
	
	void TileShadowCastersBuffer::Draw(const LightInfo& lightInfo) const
	{
		BindShadowShader();
		
		glm::ivec2 regRangeLo(glm::floor((lightInfo.m_position - lightInfo.m_range) / static_cast<float>(REGION_SIZE)));
		glm::ivec2 regRangeHi(glm::ceil((lightInfo.m_position + lightInfo.m_range) / static_cast<float>(REGION_SIZE)));
		
		regRangeLo = glm::clamp(regRangeLo, glm::ivec2(0), glm::ivec2(m_data.numRegionsX, m_data.numRegionsY));
		regRangeHi = glm::clamp(regRangeHi, glm::ivec2(0), glm::ivec2(m_data.numRegionsX - 1, m_data.numRegionsY - 1));
		
		m_vertexInputState.Bind();
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_data.indexBuffer.GetID());
		
		for (int ry = regRangeLo.y; ry <= regRangeHi.y; ry++)
		{
			uint32_t regRowBegin = m_data.numRegionsX * (uint32_t)ry;
			uint32_t firstIndex = m_data.regionRanges[regRowBegin + regRangeLo.x].firstIndex;
			uint32_t lastIndex = m_data.regionRanges[regRowBegin + regRangeHi.x].lastIndex;
			if (lastIndex > firstIndex)
			{
				uintptr_t indexBufferOffset = static_cast<uintptr_t>(firstIndex * sizeof(uint32_t));
				glDrawElements(GL_TRIANGLES, lastIndex - firstIndex, GL_UNSIGNED_INT, reinterpret_cast<void*>(indexBufferOffset));
			}
		}
	}
}
