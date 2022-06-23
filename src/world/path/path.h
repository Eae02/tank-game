#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "../../graphics/gl/buffer.h"

namespace TankGame
{
	class Path
	{
	public:
		inline Path() : m_totalLength(0) { }
		
		template <typename IteratorTp>
		static Path FromPoints(IteratorTp begin, IteratorTp end)
		{
			Path path;
			
			path.m_nodes.resize(std::distance(begin, end));
			for (IteratorTp it = begin; it != end; ++it)
				path.m_nodes[it - begin].m_position = *it;
			
			path.InitDistances();
			
			return path;
		}
		
		class PositionFromProgressResult
		{
		public:
			glm::vec2 m_position;
			glm::vec2 m_forward;
		};
		
		PositionFromProgressResult GetPositionFromProgress(float progress) const;
		float ModulateProgress(float progress) const;
		
		void AddNode(glm::vec2 position);
		void InsertNode(glm::vec2 position, size_t index);
		
		inline size_t GetNodeCount() const
		{ return m_nodes.size(); }
		
		inline glm::vec2 operator[](size_t index) const
		{ return m_nodes[index].m_position; }
		inline glm::vec2& operator[](size_t index)
		{ return m_nodes[index].m_position; }
		
		void RemoveNodes(size_t firstIndex, size_t count);
		
		inline void Close()
		{ AddNode(m_nodes.front().m_position); }
		
		class ClosestPointOnPathResult
		{
		public:
			glm::vec2 m_position;
			float m_progress;
		};
		
		ClosestPointOnPathResult GetClosestPointOnPath(glm::vec2 point) const;
		
		inline float GetTotalLength() const
		{ return m_totalLength; }
		
	private:
		void InitDistances();
		
		class Node
		{
		public:
			float m_distanceToNext;
			glm::vec2 m_position;
			
			inline Node() { }
			
			explicit inline Node(const glm::vec2& position)
			    : m_distanceToNext(0), m_position(position) { }
		};
		
		std::vector<Node> m_nodes;
		float m_totalLength;
	};
}
