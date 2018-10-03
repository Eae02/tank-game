#include "circle.h"
#include "utils/mathutils.h"

#include <cmath>
#include <limits>

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
			intersectInfo.m_penetration = (1.0f - radiiSum / std::sqrt(distSq)) * toCenterOther; 
		}
		
		return intersectInfo;
	}
	
	float Circle::GetRayIntersectionDistance(glm::vec2 start, glm::vec2 direction) const
	{
		glm::vec2 toCenter = start - m_center;
		
		float a = LengthSquared(direction);
		float b = 2 * glm::dot(direction, toCenter);
		float c = LengthSquared(toCenter) - m_radius * m_radius;
		
		float det = b * b - 4 * a * c;
		if (det < 0)
			return std::numeric_limits<float>::quiet_NaN();
		
		float dist1 = (-b + std::sqrt(det)) / (2 * a);
		float dist2 = (-b - std::sqrt(det)) / (2 * a);
		
		if (dist1 < 0 && dist2 < 0)
			return std::numeric_limits<float>::quiet_NaN();
		
		if (dist1 < 0)
			return dist2;
		if (dist2 < 0)
			return dist1;
		
		return glm::min(dist1, dist2);
	}
}
