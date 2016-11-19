#include "pathfinder.h"
#include "tilegrid.h"
#include "gameworld.h"
#include "../graphics/tilegridmaterial.h"
#include "../utils/utils.h"

#include <glm/gtc/constants.hpp>

namespace TankGame
{
	bool PathFinder::FindPathI(const TileGrid& tileGrid, const TileGridMaterial& material,
	                           glm::ivec2 start, glm::ivec2 end, Path& pathOut) const
	{
		const glm::ivec2 neighborNodeOffsets[] =
		{
			{ -1, 0 }, {  1, 0 }, { 0, -1 }, { 0,  1 }, { -1, -1 }, { -1,  1 }, {  1, -1 }, {  1,  1 },
		};
		
		const float neighborNodeDistances[] =
		{
			1, 1, 1, 1, glm::root_two<float>(), glm::root_two<float>(), glm::root_two<float>(), glm::root_two<float>()
		};
		
		m_nodes.clear();
		
		glm::ivec2 currentNode = start;
		size_t currentPathLen = 0;
		float costToCurrentNode = 0;
		
		m_nodes.insert(std::make_pair(start, Node(0, { 0, 0 }, 0)));
		
		//Uses A* to find a path across the grid
		while (true)
		{
			for (size_t i = 0; i < ArrayLength(neighborNodeOffsets); i++)
			{
				glm::ivec2 neighbor = currentNode + neighborNodeOffsets[i];
				
				if (neighbor.x < 0 || neighbor.y < 0 || neighbor.x >= tileGrid.GetWidth() || neighbor.y >= tileGrid.GetHeight())
					continue;
				if (material.IsSolid(tileGrid.GetTileID(neighbor.x, neighbor.y)))
					continue;
				
				if (i >= 4)
				{
					if (material.IsSolid(tileGrid.GetTileID(currentNode.x, neighbor.y)))
						continue;
					if (material.IsSolid(tileGrid.GetTileID(neighbor.x, currentNode.y)))
						continue;
				}
				
				auto neighborPos = m_nodes.find(neighbor);
				if (neighborPos != m_nodes.end() && neighborPos->second.m_visited)
					continue;
				
				float heuristic = glm::length(glm::vec2(neighbor - end));
				float costToNeighbor = neighborNodeDistances[i] + costToCurrentNode + heuristic;
				
				if (neighborPos == m_nodes.end())
				{
					m_nodes.insert(std::make_pair(neighbor, Node(costToNeighbor, currentNode, currentPathLen + 1)));
				}
				else if (costToNeighbor < neighborPos->second.m_lowestCost)
				{
					neighborPos->second.m_lowestCost = costToNeighbor;
					neighborPos->second.m_source = currentNode;
					neighborPos->second.m_pathLength = currentPathLen + 1;
				}
			}
			
			float lowestCost;
			decltype(m_nodes.begin()) lowestCostNodeIterator;
			bool foundNewNode = false;
			
			for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
			{
				if ((!foundNewNode || it->second.m_lowestCost < lowestCost) && !it->second.m_visited)
				{
					lowestCost = it->second.m_lowestCost;
					lowestCostNodeIterator = it;
					foundNewNode = true;
				}
			}
			
			if (!foundNewNode)
				return false;
			
			m_nodes.find(currentNode)->second.m_visited = true;
			currentNode = lowestCostNodeIterator->first;
			currentPathLen = lowestCostNodeIterator->second.m_pathLength;
			
			if (currentNode == end)
			{
				std::vector<glm::vec2> path(currentPathLen + 1);
				
				path[currentPathLen] = glm::vec2(currentNode) + glm::vec2(0.5f);
				
				auto sourceNodeIterator = m_nodes.find(lowestCostNodeIterator->second.m_source);
				
				if (sourceNodeIterator == m_nodes.end())
				{
					GetLogStream() << "[error] Pathfinder error. Source node does not exist.\n";
					return false;
				}
				
				long index = currentPathLen - 1;
				
				//Reconstructs the path back
				while (true)
				{
					path[index--] = glm::vec2(sourceNodeIterator->first) + glm::vec2(0.5f);
					if (sourceNodeIterator->first == start)
						break;
					assert(index >= 0);
					
					sourceNodeIterator = m_nodes.find(sourceNodeIterator->second.m_source);
					assert(sourceNodeIterator != m_nodes.end());
				}
				
				assert(index == -1);
				
				pathOut = Path::FromPoints(path.begin(), path.end());
				return true;
			}
		}
	}
	
	static bool IsSphereRayObstructed(const TileGrid& tileGrid, const TileGridMaterial& material,
	                                  glm::vec2 start, glm::vec2 end, float radius)
	{
		glm::vec2 toEnd = glm::normalize(end - start);
		glm::vec2 orthoRay(-toEnd.y * radius, toEnd.x * radius);
		
		return tileGrid.IsRayObstructed(material, start + orthoRay, end + orthoRay) ||
		       tileGrid.IsRayObstructed(material, start - orthoRay, end - orthoRay);
	}
	
	bool PathFinder::FindPath(const TileGrid& tileGrid, const TileGridMaterial& material, glm::vec2 start,
	                          glm::vec2 end, class Path& pathOut, float radius) const
	{
		if (!IsSphereRayObstructed(tileGrid, material, start, end, radius))
		{
			pathOut.AddNode(start);
			pathOut.AddNode(end);
			return true;
		}
		
		if (!FindPathI(tileGrid, material, { std::floor(start.x), std::floor(start.y) },
		               { std::floor(end.x), std::floor(end.y) }, pathOut))
		{
			return false;
		}
		
		for (size_t i = 1; i < pathOut.GetNodeCount(); i++)
		{
			if (IsSphereRayObstructed(tileGrid, material, start, pathOut[i], radius))
			{
				pathOut.RemoveNodes(0, i - 1);
				pathOut.InsertNode(start, 0);
				break;
			}
		}
		
		return true;
	}
	
	bool PathFinder::FindPath(const GameWorld& gameWorld, glm::vec2 start, glm::vec2 end,
	                          class Path& pathOut, float radius) const
	{
		return FindPath(*gameWorld.GetTileGrid(), *gameWorld.GetTileGridMaterial(), start, end, pathOut, radius);
	}
	
	static thread_local PathFinder instance;
	const PathFinder& PathFinder::GetInstance()
	{ return instance; }
}
