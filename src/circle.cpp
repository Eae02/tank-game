#include "circle.h"
#include <cmath>

namespace TankGame
{
	bool Circle::Intersects(const Circle& other) const
	{
		glm::vec2 centerDiff = other.m_center - m_center;
		float distSq = centerDiff.x * centerDiff.x + centerDiff.y * centerDiff.y;
		
		float radiiSum = m_radius + other.m_radius;
		
		return distSq <= radiiSum * radiiSum;
	}
	
	bool Circle::Contains(glm::vec2 point) const
	{
		glm::vec2 toPoint = point - m_center;
		return toPoint.x * toPoint.x + toPoint.y * toPoint.y <= m_radius;
	}
	
	IntersectInfo Circle::GetIntersectInfo(const Circle& other) const
	{
		glm::vec2 toCenterOther = other.m_center - m_center;
		float distSq = toCenterOther.x * toCenterOther.x + toCenterOther.y * toCenterOther.y;
		
		float radiiSum = m_radius + other.m_radius;
		
		IntersectInfo intersectInfo;
		intersectInfo.m_intersects = distSq <= radiiSum * radiiSum;
		
		if (intersectInfo.m_intersects)
		{
			intersectInfo.m_penetration = (radiiSum / std::sqrt(distSq) - 1.0f) * toCenterOther; 
		}
		
		return intersectInfo;
	}
}
