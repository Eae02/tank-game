#include "tilegrid.h"
#include "../graphics/tilegridmaterial.h"
#include "../graphics/quadmesh.h"
#include "../graphics/viewinfo.h"
#include "../graphics/gl/shadermodule.h"
#include "../graphics/shadowrenderer.h"
#include "../utils/utils.h"
#include "../utils/ioutils.h"

#include <iostream>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/fast_square_root.hpp>

#include <unordered_map>

namespace TankGame
{
	TileGrid::TileGrid(int width, int height)
	    : m_width(width), m_height(height),
	      m_gridData(width * height, 0), m_rotationData(width * height, glm::vec2(1, 0)),
	      m_idsTexture(width, height, 1, GL_R8UI), m_rotationsTexture(width, height, 1, GL_RG32F)
	{
		m_idsTexture.SetMinFilter(GL_NEAREST);
		m_idsTexture.SetMagFilter(GL_NEAREST);
		m_idsTexture.SetWrapMode(GL_CLAMP_TO_EDGE);
		
		m_rotationsTexture.SetMinFilter(GL_NEAREST);
		m_rotationsTexture.SetMagFilter(GL_NEAREST);
	}
	
	void TileGrid::SetTileID(glm::ivec2 pos, uint8_t tileID)
	{
		if (pos.x >= m_width || pos.y >= m_height || pos.x < 0 || pos.y < 0)
			throw std::runtime_error("Tile index out of range.");
		m_gridData[pos.x + pos.y * m_width] = tileID;
	}
	
	void TileGrid::SetTileRotation(glm::ivec2 pos, float rotation)
	{
		if (pos.x >= m_width || pos.y >= m_height || pos.x < 0 || pos.y < 0)
			throw std::runtime_error("Tile index out of range.");
		
		m_rotationData[pos.x + pos.y * m_width] = { std::cos(rotation), std::sin(rotation) };
	}
	
	uint8_t TileGrid::GetTileID(glm::ivec2 pos) const
	{
		if (pos.x > m_width || pos.y > m_height || pos.x < 0 || pos.y < 0)
			throw std::runtime_error("Tile index out of range.");
		return m_gridData[pos.x + pos.y * m_width];
	}
	
	void TileGrid::Draw(const ViewInfo& viewInfo, const TileGridMaterial& material) const
	{
		glm::ivec2 minTile = {
			glm::clamp<int>(static_cast<int>(std::floor(viewInfo.GetViewRectangle().x)), 0, m_width),
			glm::clamp<int>(static_cast<int>(std::floor(viewInfo.GetViewRectangle().y)), 0, m_height),
		};
		
		glm::ivec2 maxTile = {
			glm::clamp<int>(static_cast<int>(std::ceil(viewInfo.GetViewRectangle().FarX())), 0, m_width),
			glm::clamp<int>(static_cast<int>(std::ceil(viewInfo.GetViewRectangle().FarY())), 0, m_height),
		};
		
		glm::ivec2 renderSize = maxTile - minTile;
		if (renderSize.x == 0 || renderSize.y == 0)
			return;
		
		QuadMesh::GetInstance().GetVAO().Bind();
		
		m_idsTexture.Bind(0);
		m_rotationsTexture.Bind(1);
		
		material.Bind(minTile, maxTile);
		
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, renderSize.x * renderSize.y);
	}
	
	void TileGrid::UploadGridData(int x, int y, int width, int height)
	{
		for (int i = 0; i < height; i++)
		{
			size_t index = x + (y + i) * m_width;
			
			glTextureSubImage2D(m_idsTexture.GetID(), 0, x, y + i, width, 1,
			                    GL_RED_INTEGER, GL_UNSIGNED_BYTE, &m_gridData[index]);
			
			glTextureSubImage2D(m_rotationsTexture.GetID(), 0, x, y + i, width, 1,
			                    GL_RG, GL_FLOAT, &m_rotationData[index]);
		}
	}
	
	void TileGrid::UploadGridData()
	{
		glTextureSubImage2D(m_idsTexture.GetID(), 0, 0, 0, m_width, m_height,
		                    GL_RED_INTEGER, GL_UNSIGNED_BYTE, &m_gridData[0]);
		
		glTextureSubImage2D(m_rotationsTexture.GetID(), 0, 0, 0, m_width, m_height,
		                    GL_RG, GL_FLOAT, &m_rotationData[0]);
	}
	
	IntersectInfo TileGrid::GetIntersectInfo(const TileGridMaterial& material, const Circle& circle) const
	{
		glm::ivec2 start(std::floor(circle.GetCenter().x - circle.GetRadius()),
		                 std::floor(circle.GetCenter().y - circle.GetRadius()));
		glm::ivec2 end(std::floor(circle.GetCenter().x + circle.GetRadius()),
		               std::floor(circle.GetCenter().y + circle.GetRadius()));
		
		start = glm::clamp(start, {0, 0}, {m_width - 1, m_height - 1});
		end = glm::clamp(end, {0, 0}, {m_width - 1, m_height - 1});
		
		float radiusSq = circle.GetRadius() * circle.GetRadius();
		
		float lowestDistToCenterSq = std::numeric_limits<float>::infinity();
		
		IntersectInfo intersectInfo;
		
		for (int y = start.y; y <= end.y; y++)
		{
			for (int x = start.x; x <= end.x; x++)
			{
				if (!material.IsSolid(GetTileID({ x, y })))
					continue;
				
				if (circle.GetCenter().x > x && circle.GetCenter().y > y &&
				    circle.GetCenter().x < x + 1 && circle.GetCenter().y < y + 1)
				{
					intersectInfo.m_intersects = true;
					
					glm::vec2 toTileCenter = circle.GetCenter() - glm::vec2(x + 0.5f, y + 0.5f);
					float toTileCenterLen = glm::length(toTileCenter);
					
					float minComp = std::min(std::abs(toTileCenter.x), std::abs(toTileCenter.y));
					float sinTheta = minComp / toTileCenterLen;
					float distToEdge = glm::fastInverseSqrt(1.0f - sinTheta * sinTheta) / 2.0f;
					
					float penetrationDist = distToEdge - (toTileCenterLen - circle.GetRadius());
					intersectInfo.m_penetration = -toTileCenter * (penetrationDist / toTileCenterLen);
					
					return intersectInfo;
				}
				
				glm::vec2 penetrationPoint = glm::clamp(circle.GetCenter(), {x, y}, {x + 1.0f, y + 1.0f});
				glm::vec2 centerToPP = penetrationPoint - circle.GetCenter();
				
				float distToCenterSq = centerToPP.x * centerToPP.x + centerToPP.y * centerToPP.y;
				
				if (distToCenterSq < radiusSq)
				{
					if (intersectInfo.m_intersects && distToCenterSq > lowestDistToCenterSq)
						continue;
					
					float distToCenter = std::sqrt(distToCenterSq);
					intersectInfo.m_penetration = glm::normalize(centerToPP) * (circle.GetRadius() - distToCenter);
					intersectInfo.m_intersects = true;
					
					lowestDistToCenterSq = distToCenterSq;
				}
			}
		}
		
		return intersectInfo;
	}
	
	float TileGrid::GetRayIntersectionDistance(const TileGridMaterial& material, glm::vec2 start, glm::vec2 end) const
	{
		float distance = 0;
		
		int firstHlCross = static_cast<int>(std::ceil(std::min(start.y, end.y)));
		int lastHlCross = static_cast<int>(std::floor(std::max(start.y, end.y)));
		
		glm::vec2 startToEnd = end - start;
		
		for (int row = firstHlCross - 1; row <= lastHlCross; row++)
		{
			if (row < 0 || row >= m_height)
				continue;
			
			int thisHlY = row;
			int nextHlY = row + 1;
			
			float enterDist = 0;
			float exitDist = 1;
			
			if (std::abs(startToEnd.y) > 1E-6)
			{
				enterDist = glm::clamp((thisHlY - start.y) / startToEnd.y, 0.0f, 1.0f);
				exitDist = glm::clamp((nextHlY - start.y) / startToEnd.y, 0.0f, 1.0f);
			}
			
			float minRowDist = std::min(enterDist, exitDist);
			float maxRowDist = std::max(enterDist, exitDist);
			
			//Finds the x coordinates at which the ray enters this row
			float enterX = start.x + enterDist * startToEnd.x;
			float exitX = start.x + exitDist * startToEnd.x;
			
			int firstVlCross = static_cast<int>(std::ceil(std::min(enterX, exitX)));
			int lastVlCross = static_cast<int>(std::floor(std::max(enterX, exitX)));
			
			for (int col = firstVlCross - 1; col <= lastVlCross; col++)
			{
				if (col < 0 || col >= m_width || !material.IsSolid(GetTileID({ col, row })))
					continue;
				
				int thisVlX = col;
				int nextVlX = col + 1;
				
				float cellEnterDist = 0;
				float cellExitDist = 1;
				
				if (std::abs(startToEnd.x) > 1E-6)
				{
					cellEnterDist = glm::clamp((thisVlX - start.x) / startToEnd.x, minRowDist, maxRowDist);
					cellExitDist = glm::clamp((nextVlX - start.x) / startToEnd.x, minRowDist, maxRowDist);
				}
				
				distance += std::abs(cellExitDist - cellEnterDist);
			}
		}
		
		return distance * glm::length(startToEnd);
	}
	
	//TODO: Remove code duplication
	bool TileGrid::IsRayObstructed(const class TileGridMaterial& material, glm::vec2 start, glm::vec2 end) const
	{
		int firstHlCross = static_cast<int>(std::ceil(std::min(start.y, end.y)));
		int lastHlCross = static_cast<int>(std::floor(std::max(start.y, end.y)));
		
		glm::vec2 startToEnd = end - start;
		
		for (int row = firstHlCross - 1; row <= lastHlCross; row++)
		{
			if (row < 0 || row >= m_height)
				continue;
			
			int thisHlY = row;
			int nextHlY = row + 1;
			
			float enterDist = 0;
			float exitDist = 1;
			
			if (std::abs(startToEnd.y) > 1E-6)
			{
				enterDist = glm::clamp((thisHlY - start.y) / startToEnd.y, 0.0f, 1.0f);
				exitDist = glm::clamp((nextHlY - start.y) / startToEnd.y, 0.0f, 1.0f);
			}
			
			//Finds the x coordinates at which the ray enters this row
			float enterX = start.x + enterDist * startToEnd.x;
			float exitX = start.x + exitDist * startToEnd.x;
			
			int firstVlCross = static_cast<int>(std::ceil(std::min(enterX, exitX)));
			int lastVlCross = static_cast<int>(std::floor(std::max(enterX, exitX)));
			
			for (int col = firstVlCross - 1; col <= lastVlCross; col++)
			{
				if (col >= 0 && col < m_width && material.IsSolid(GetTileID({ col, row })))
					return true;
			}
		}
		
		return false;
	}
	
	static float FindIntStep(float s, float ds)
	{
		if (ds < 0)
			return FindIntStep(-s, -ds);
		
		return (1.0f - glm::fract(s)) / ds;
	}
	
	float TileGrid::GetDistanceToWall(const TileGridMaterial& material, glm::vec2 position, glm::vec2 forward) const
	{
		glm::ivec2 coords(std::floor(position.x), std::floor(position.y));
		
		glm::ivec2 step(glm::sign(forward.x), glm::sign(forward.y));
		
		glm::vec2 maxT(FindIntStep(position.x, forward.x), FindIntStep(position.y, forward.y));
		glm::vec2 deltaT(step.x / forward.x, step.y / forward.y);
		
		int enterStepDir = 0;
		float enterLineOffset = 0;
		
		while (true)
		{
			bool solid = InRange(coords) && material.IsSolid(GetTileID(coords));
			
			if ((solid && enterStepDir == 1) || (step.y > 0 ? coords.y >= m_height : coords.y < 0))
			{
				float y = solid ? (coords.y + 0.5f + enterLineOffset) : (step.y > 0 ? m_height : 0);
				
				float dy = position.y - y;
				float dx = dy * forward.x / forward.y;
				
				return std::sqrt(dy * dy + dx * dx);
			}
			
			if ((solid && enterStepDir == 0) || (step.x > 0 ? coords.x >= m_width : coords.x < 0))
			{
				float x = solid ? (coords.x + 0.5f + enterLineOffset) : (step.x > 0 ? m_width : 0);
				
				float dx = position.x - x;
				float dy = dx * forward.y / forward.x;
				
				return std::sqrt(dy * dy + dx * dx);
			}
			
			int stepDir;
			
			if (maxT.x < maxT.y)
				stepDir = 0;
			else
				stepDir = 1;
			
			int s = glm::sign(forward[stepDir]);
			
			coords[stepDir] += s;
			maxT[stepDir] += deltaT[stepDir];
			
			enterStepDir = stepDir;
			enterLineOffset = s * -0.5f;
		}
	}
}
