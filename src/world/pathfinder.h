#pragma once

#include <glm/glm.hpp>

#include <set>
#include <unordered_map>

namespace std
{
	template <>
	struct hash<glm::ivec2>
	{
		inline size_t operator()(glm::ivec2 v) const
		{
			hash<int> iHash;
			return iHash(v.x) ^ iHash(v.y);
		}
	};
}

namespace TankGame
{
	//This class is not thread safe!
	class PathFinder
	{
	public:
		bool FindPathI(const class TileGrid& tileGrid, const class TileGridMaterial& material,
		               glm::ivec2 start, glm::ivec2 end, class Path& pathOut) const;
		
		bool FindPath(const class TileGrid& tileGrid, const class TileGridMaterial& material,
		              glm::vec2 start, glm::vec2 end, class Path& pathOut, float radius) const;
		
		bool FindPath(const class GameWorld& gameWorld, glm::vec2 start, glm::vec2 end,
		              class Path& pathOut, float radius) const;
		
		static const PathFinder& GetInstance();
		
	private:
		struct Node
		{
			float m_lowestCost;
			glm::ivec2 m_source;
			bool m_visited;
			size_t m_pathLength;
			
			inline Node() { }
			
			inline Node(float lowestCost, const glm::ivec2& source, size_t pathLength)
			    : m_lowestCost(lowestCost), m_source(source), m_visited(false), m_pathLength(pathLength) { }
		};
		
		mutable std::unordered_map<glm::ivec2, Node> m_nodes;
	};
}
