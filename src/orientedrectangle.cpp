#include "orientedrectangle.h"

namespace TankGame
{
	OrientedRectangle::OrientedRectangle()
	    : m_edgeNormals{ { 0, 1 }, { 0, -1 }, { 1, 0 }, { -1, 0 } }
	{
		std::fill(std::begin(m_edgePositions), std::end(m_edgePositions), glm::vec2(0.0f));
	}
	
	OrientedRectangle::OrientedRectangle(glm::vec2 center, glm::vec2 size, float rotation)
	{
		glm::vec2 forward(std::sin(rotation), -std::cos(rotation));
		
		m_edgeNormals[0] = forward;
		m_edgeNormals[1] = -forward;
		m_edgeNormals[2] = { forward.y, -forward.x };
		m_edgeNormals[3] = { -forward.y, forward.x };
		
		for (int i = 0; i < 4; i++)
		{
			m_edgePositions[i] = center + m_edgeNormals[i] * size[1 - i / 2];
		}
	}
	
	IntersectInfo OrientedRectangle::GetIntersectInfo(const Circle& circle)
	{
		IntersectInfo intersectInfo;
		intersectInfo.m_intersects = true;
		
		float minPenetrationDist = std::numeric_limits<float>::infinity();
		
		for (int i = 0; i < 4; i++)
		{
			glm::vec2 toCenterCircle = circle.GetCenter() - m_edgePositions[i];
			
			float distToCircleEdge = glm::dot(toCenterCircle, m_edgeNormals[i]) - circle.GetRadius();
			
			if (distToCircleEdge > 0)
				return { };
			
			float penetration = -distToCircleEdge;
			if (penetration < minPenetrationDist)
			{
				minPenetrationDist = penetration;
				intersectInfo.m_penetration = distToCircleEdge * m_edgeNormals[i];
			}
		}
		
		return intersectInfo;
	}
}
