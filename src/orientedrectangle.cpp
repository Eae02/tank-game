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
		
		glm::vec2 corners[4];
		
		for (int i = 0; i < 4; i++)
		{
			m_edgePositions[i] = center + m_edgeNormals[i] * size[1 - i / 2];
			corners[i] = m_edgePositions[i] + m_edgeNormals[(i + 2) % 4] * size[i / 2];
		}
		
		float minX = corners[0].x;
		float maxX = corners[0].x;
		float minY = corners[0].y;
		float maxY = corners[0].y;
		
		for (int i = 1; i < 3; i++)
		{
			minX = std::min(minX, corners[i].x);
			maxX = std::max(maxX, corners[i].x);
			minY = std::min(minY, corners[i].y);
			maxY = std::max(maxY, corners[i].y);
		}
		
		m_outsideRectangle = Rectangle::FromMinMax({ minX, minY }, { maxX, maxY });
	}
	
	bool OrientedRectangle::Contains(glm::vec2 point) const
	{
		if (!m_outsideRectangle.Contains(point))
			return false;
		
		for (int i = 0; i < 4; i++)
		{
			float distToPoint = glm::dot(point - m_edgePositions[i], m_edgeNormals[i]);
			
			if (distToPoint > 0)
				return false;
		}
		
		return true;
	}
	
	IntersectInfo OrientedRectangle::GetIntersectInfo(const Circle& circle) const
	{
		if (!m_outsideRectangle.Intersects(circle.GetBoundingRectangle()))
			return { };
		
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
	
	float OrientedRectangle::GetRayIntersectionDistance(glm::vec2 start, glm::vec2 direction) const
	{
		float minDistAlongRay = std::numeric_limits<float>::quiet_NaN();
		
		for (int i = 0; i < 4; i++)
		{
			float q = glm::dot(direction, m_edgeNormals[i]);
			if (std::abs(q) < 1E-6f)
				continue;
			
			float distAlongRay = glm::dot(m_edgePositions[i] - start, m_edgeNormals[i]) / q;
			if (distAlongRay < 0)
				continue;
			
			glm::vec2 positionOnLine = start + distAlongRay * direction;
			size_t firstOtherEdge = i < 2 ? 2 : 0;
			
			float distToPoint1 = glm::dot(positionOnLine - m_edgePositions[firstOtherEdge], m_edgeNormals[firstOtherEdge]);
			float distToPoint2 = glm::dot(positionOnLine - m_edgePositions[firstOtherEdge + 1], m_edgeNormals[firstOtherEdge + 1]);
			
			if (distToPoint1 > 0 || distToPoint2 > 0)
				continue;
			
			if (std::isnan(minDistAlongRay) || distAlongRay < minDistAlongRay)
				minDistAlongRay = distAlongRay;
		}
		
		return minDistAlongRay;
	}
}
