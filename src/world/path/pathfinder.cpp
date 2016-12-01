#include "pathfinder.h"
#include "../gameworld.h"
#include "../tilegrid.h"
#include "../../utils/mathutils.h"

#include <unordered_map>

namespace std
{
	template<> struct hash<glm::ivec2>
	{
		size_t operator()(const glm::ivec2& v) const
		{
			return (static_cast<size_t>(v.x) * 9973) + static_cast<size_t>(v.y);
		}
	};
}

namespace TankGame
{
	struct Node
	{
		glm::ivec2 m_toParent;
		bool m_closed = false;
		
		float m_movementCost;
		float m_completionCost;
		
		Node() = default;
		
		Node(glm::ivec2 toParent, float movementCost, float completionCost)
		    : m_toParent(toParent), m_movementCost(movementCost), m_completionCost(completionCost) { }
	};
	
	struct NeighborNode
	{
		glm::ivec2 m_toParent;
		float m_distance;
		
		NeighborNode(glm::ivec2 toParent)
		    : m_toParent(toParent), m_distance(glm::length(glm::vec2(toParent))) { }
	};
	
	const NeighborNode neighborNodes[] = 
	{
		NeighborNode({ -1, -1 }), NeighborNode({ 0, -1 }), NeighborNode({ 1, -1 }),
		NeighborNode({ -1,  0 }),                          NeighborNode({ 1,  0 }),
		NeighborNode({ -1,  1 }), NeighborNode({ 0,  1 }), NeighborNode({ 1,  1 })
	};
	
	std::vector<glm::ivec2> lastPath;
	std::vector<std::array<glm::vec2, 2>> lastPathPortals;
	
	static float CrossProd2D(glm::vec2 a, glm::vec2 b)
	{
		glm::vec2 aL(a.y, -a.x);
		
		return glm::dot(aL, b);
		
		//return b.x * a.y - a.x * b.y;
	}
	
	static bool Vec2Equal(glm::vec2 a, glm::vec2 b)
	{
		return LengthSquared(a - b) < 1E-6f;
	}
	
	bool FindPath(const TileGrid& tileGrid, const TileGridMaterial& material, glm::vec2 start, glm::vec2 end,
	              Path& pathOut, float radius)
	{
		std::unordered_map<glm::ivec2, Node> nodes;
		
		glm::ivec2 startTile(std::floor(start.x), std::floor(start.y));
		glm::ivec2 endTile(std::floor(end.x), std::floor(end.y));
		
		//Adds the start node to the open set
		nodes.emplace(startTile, Node(glm::ivec2(0), 0.0f, glm::distance(glm::vec2(startTile), end)));
		
		glm::ivec2 currentNode = startTile;
		
		//Uses A* to find a path across the grid
		while (currentNode != endTile)
		{
			//Iterates this node's neighbors
			for (const NeighborNode& neighbor : neighborNodes)
			{
				glm::ivec2 neighborPos = currentNode - neighbor.m_toParent;
				
				if (!tileGrid.InRange(neighborPos) || material.IsSolid(tileGrid.GetTileID(neighborPos)))
					continue;
				
				glm::ivec2 passThrough[2] = { { neighborPos.x, currentNode.y }, { currentNode.x, neighborPos.y } };
				if (std::any_of(passThrough, passThrough + 2, [&] (glm::ivec2 p) { return material.IsSolid(tileGrid.GetTileID(p)); }))
					continue;
				
				//The movement cost (G term) for this neighbor when comming from the current node.
				float movementCost = nodes.at(currentNode).m_movementCost + neighbor.m_distance;
				
				auto it = nodes.find(neighborPos);
				
				if (it == nodes.end())
				{
					nodes.emplace(neighborPos, Node(neighbor.m_toParent, movementCost,
					                                glm::distance(glm::vec2(neighborPos), end)));
				}
				else if (movementCost < it->second.m_movementCost)
				{
					it->second.m_movementCost = movementCost;
					it->second.m_toParent = neighbor.m_toParent;
				}
			}
			
			nodes.at(currentNode).m_closed = true;
			
			//Searches for the node with the lowest cost and makes it the new current node.
			float lowestCost = std::numeric_limits<float>::quiet_NaN();
			for (const std::pair<glm::ivec2, Node>& node : nodes)
			{
				if (node.second.m_closed)
					continue;
				
				float cost = node.second.m_movementCost + node.second.m_completionCost;
				
				if (std::isnan(lowestCost) || cost < lowestCost)
				{
					lowestCost = cost;
					currentNode = node.first;
				}
			}
			
			if (std::isnan(lowestCost))
				return false;
		}
		
		int numPortals = 0;
		
		//Reconstructs the path back
		lastPath.clear();
		for (glm::ivec2 node = currentNode; node != startTile; node += nodes[node].m_toParent)
		{
			numPortals++;
			lastPath.push_back(node);
		}
		
		struct Portal
		{
			glm::vec2 m_l, m_r;
			Portal() = default;
			Portal(glm::vec2 l, glm::vec2 r) : m_l(l), m_r(r) { }
		};
		
		std::vector<Portal> portals(numPortals + 1);
		
		currentNode = endTile;
		long i = numPortals - 1;
		for (glm::ivec2 nodePos = endTile; nodePos != startTile;)
		{
			glm::ivec2 toParent = nodes[nodePos].m_toParent;
			
			glm::vec2 centerPortal = glm::vec2(nodePos) + glm::vec2(0.5f) + glm::vec2(toParent) * 0.5f;
			glm::ivec2 portalL(toParent.y, -toParent.x);
			
			glm::vec2 toEdge(portalL);
			
			portals[i] = { centerPortal + toEdge * 0.5f, centerPortal - toEdge * 0.5f };
			
			if (portalL.x == 0 || portalL.y == 0)
			{
				bool contractL = material.IsSolid(tileGrid.GetTileID(nodePos + portalL)) ||
				        material.IsSolid(tileGrid.GetTileID(nodePos + portalL + toParent));
				bool contractR = material.IsSolid(tileGrid.GetTileID(nodePos - portalL)) ||
				        material.IsSolid(tileGrid.GetTileID(nodePos - portalL + toParent));
				
				if (contractL)
					portals[i].m_l -= toEdge * radius;
				else
					portals[i].m_l += toEdge * radius;
				
				if (contractR)
					portals[i].m_r += toEdge * radius;
				else
					portals[i].m_r -= toEdge * radius;
			}
			
			nodePos += toParent;
			i--;
		}
		
		portals.back() = Portal(end, end);
		
		assert(i == -1);
		
		lastPathPortals.resize(portals.size());
		std::transform(portals.begin(), portals.end(), lastPathPortals.begin(), [] (const Portal& p)
		{
			return std::array<glm::vec2, 2> { p.m_l, p.m_r };
		});
		
		pathOut.AddNode(start);
		
		int lIndex = 0;
		int rIndex = 0;
		
		for (size_t i = 1; i < portals.size(); i++)
		{
			glm::vec2 apex = pathOut[pathOut.GetNodeCount() - 1];
			glm::vec2 toLeft = portals[lIndex].m_l - apex;
			glm::vec2 toRight = portals[rIndex].m_r - apex;
			
			if (!Vec2Equal(portals[lIndex].m_l, portals[i].m_l))
			{
				glm::vec2 toVertex = portals[i].m_l - apex;
				if (CrossProd2D(toVertex, toLeft) <= 0.0f)
				{
					if (CrossProd2D(toVertex, toRight) < 0.0f)
					{
						pathOut.AddNode(portals[rIndex].m_r);
						
						i = lIndex = rIndex;
						
						continue;
					}
					
					lIndex = i;
				}
			}
			
			if (!Vec2Equal(portals[rIndex].m_r, portals[i].m_r))
			{
				glm::vec2 toVertex = portals[i].m_r - apex;
				if (CrossProd2D(toVertex, toRight) >= 0.0f)
				{
					if (CrossProd2D(toVertex, toLeft) > 0.0f)
					{
						pathOut.AddNode(portals[lIndex].m_l);
						
						i = rIndex = lIndex;
						
						continue;
					}
					
					rIndex = i;
				}
			}
		}
		
		pathOut.AddNode(end);
		
		return true;
	}
	
	bool FindPath(const GameWorld& gameWorld, glm::vec2 start, glm::vec2 end, Path& pathOut, float radius)
	{
		return FindPath(*gameWorld.GetTileGrid(), *gameWorld.GetTileGridMaterial(), start, end, pathOut, radius);
	}
}
