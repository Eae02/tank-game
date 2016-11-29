#pragma once

#include <vector>
#include <glm/vec2.hpp>

#include "intersectinfo.h"
#include "path/path.h"
#include "../graphics/gl/texture2d.h"
#include "../graphics/gl/bufferallocator.h"
#include "../graphics/gl/vertexarray.h"
#include "../graphics/gl/shaderprogram.h"
#include "../circle.h"
#include "../utils/memory/stackobject.h"

namespace TankGame
{
	class TileGrid
	{
	public:
		TileGrid(int width, int height);
		
		void SetTileID(glm::ivec2 pos, uint8_t tileID);
		void SetTileRotation(glm::ivec2 pos, float rotation);
		
		uint8_t GetTileID(glm::ivec2 pos) const;
		
		void UploadGridData();
		void UploadGridData(int x, int y, int width, int height);
		
		void PrepareForRendering(const class ViewInfo& viewInfo) const;
		void Draw(const class TileGridMaterial& material) const;
		
		inline int GetWidth() const
		{ return m_width; }
		inline int GetHeight() const
		{ return m_height; }
		
		inline bool InRange(glm::ivec2 point) const
		{
			return point.x >= 0 && point.y >= 0 && point.x < m_width && point.y < m_height;
		}
		
		IntersectInfo GetIntersectInfo(const class TileGridMaterial& material, const Circle& circle) const;
		float GetRayIntersectionDistance(const class TileGridMaterial& material, glm::vec2 start, glm::vec2 end) const;
		
		bool IsRayObstructed(const class TileGridMaterial& material, glm::vec2 start, glm::vec2 end) const;
		
		float GetDistanceToWall(const class TileGridMaterial& material, glm::vec2 position, glm::vec2 forward) const;
		
		inline const uint8_t* GetDataPtr() const
		{ return m_gridData.data(); }
		inline size_t GetDataSize() const
		{ return m_gridData.size(); }
		
	private:
		int m_width;
		int m_height;
		
		mutable glm::ivec2 m_renderSize;
		
		std::vector<uint8_t> m_gridData;
		
		std::vector<glm::vec2> m_rotationData;
		
		Texture2D m_idsTexture;
		Texture2D m_rotationsTexture;
		
		BufferAllocator::UniquePtr m_renderAreaBuffer;
	};
}
