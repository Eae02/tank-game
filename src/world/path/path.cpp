#include "path.h"
#include "../../utils/utils.h"

#include <stdexcept>

namespace TankGame
{
	Path::PositionFromProgressResult Path::GetPositionFromProgress(float progress) const
	{
		float distanceLeft = glm::clamp(progress, 0.0f, m_totalLength);
		
		for (size_t i = 0; i < m_nodes.size() - 1; i++)
		{
			if (m_nodes[i].m_distanceToNext > distanceLeft)
			{
				float a = distanceLeft / m_nodes[i].m_distanceToNext;
				
				PositionFromProgressResult result;
				result.m_position = glm::mix(m_nodes[i].m_position, m_nodes[i + 1].m_position, a);
				result.m_forward = m_nodes[i + 1].m_position - m_nodes[i].m_position;
				
				return result;
			}
			
			distanceLeft -= m_nodes[i].m_distanceToNext;
		}
		
		PositionFromProgressResult result;
		result.m_position = m_nodes.back().m_position;
		result.m_forward = m_nodes.back().m_position - m_nodes[m_nodes.size() - 2].m_position;
		return result;
	}
	
	float Path::ModulateProgress(float progress) const
	{
		return progress - std::floor(progress / m_totalLength) * m_totalLength;
	}
	
	void Path::AddNode(glm::vec2 position)
	{
		if (!m_nodes.empty())
		{
			m_nodes.back().m_distanceToNext = glm::distance(m_nodes.back().m_position, position);
			if (m_nodes.back().m_distanceToNext < 1E-6)
				return;
			m_totalLength += m_nodes.back().m_distanceToNext;
		}
		m_nodes.emplace_back(position);
	}
	
	void Path::InitDistances()
	{
		m_totalLength = 0;
		
		for (size_t i = 1; i < m_nodes.size(); i++)
		{
			m_nodes[i - 1].m_distanceToNext = glm::distance(m_nodes[i - 1].m_position, m_nodes[i].m_position);
			m_totalLength += m_nodes[i - 1].m_distanceToNext;
		}
		
		m_nodes.back().m_distanceToNext = 0.0f;
	}
	
	void Path::RemoveNodes(size_t firstIndex, size_t count)
	{
		for (size_t i = 0; i < count; i++)
			m_totalLength -= m_nodes[firstIndex + i].m_distanceToNext;
		
		m_nodes.erase(m_nodes.begin() + firstIndex, m_nodes.begin() + firstIndex + count);
		if (firstIndex != 0)
		{
			float newDistanceToNext = glm::distance(m_nodes[firstIndex - 1].m_position, m_nodes[firstIndex].m_position);
			
			m_totalLength += newDistanceToNext - m_nodes[firstIndex - 1].m_distanceToNext;
			m_nodes[firstIndex - 1].m_distanceToNext = newDistanceToNext;
		}
	}
	
	void Path::InsertNode(glm::vec2 position, size_t index)
	{
		Node newNode(position);
		if (index != m_nodes.size())
		{
			newNode.m_distanceToNext = glm::distance(position, m_nodes[index].m_position);
			m_totalLength += newNode.m_distanceToNext;
		}
		
		if (index != 0)
		{
			float distanceToPre = glm::distance(position, m_nodes[index - 1].m_position);
			
			m_totalLength += distanceToPre - m_nodes[index - 1].m_distanceToNext;
			m_nodes[index - 1].m_distanceToNext = distanceToPre;
		}
		
		m_nodes.insert(m_nodes.begin() + index, newNode);
	}
	
	Path::ClosestPointOnPathResult Path::GetClosestPointOnPath(glm::vec2 point) const
	{
		if (m_nodes.empty())
			Panic("Path cannot be empty.");
		
		float closestDistanceToLine = std::numeric_limits<float>::max();
		
		ClosestPointOnPathResult result;
		result.m_position = m_nodes[0].m_position;
		result.m_progress = 0.0f;
		
		float progress = 0.0f;
		
		for (size_t i = 0; i < m_nodes.size() - 1; i++)
		{
			glm::vec2 lineBegin = m_nodes[i].m_position;
			glm::vec2 lineEnd = m_nodes[i + 1].m_position;
			
			float lineLength = glm::distance(lineBegin, lineEnd);
			
			glm::vec2 toNext = lineEnd - lineBegin;
			glm::vec2 normal = glm::vec2(-toNext.y, toNext.x) / lineLength;
			
			float distanceToLine = glm::dot(normal, point - lineBegin);
			float absDistToLine = glm::abs(distanceToLine);
			
			glm::vec2 pointOnLine = point - normal * distanceToLine;
			
			int dim = std::abs(toNext.y) > std::abs(toNext.x) ? 1 : 0;
			float distanceOnLine = (pointOnLine[dim] - lineBegin[dim]) / toNext[dim];
			
			if (distanceOnLine < 0 || distanceOnLine > 1)
			{
				distanceOnLine = glm::clamp(distanceOnLine, 0.0f, 1.0f);
				pointOnLine = lineBegin + distanceOnLine * toNext;
				absDistToLine = glm::distance(pointOnLine, point);
			}
			
			if (absDistToLine < closestDistanceToLine)
			{
				result.m_position = pointOnLine;
				result.m_progress = progress + distanceOnLine * lineLength;
				closestDistanceToLine = absDistToLine;
			}
			
			progress += lineLength;
		}
		
		return result;
	}
}
