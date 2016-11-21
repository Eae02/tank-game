#include "colliderinfo.h"

#include <limits>

namespace TankGame
{
	IntersectInfo ColliderInfo::GetIntersectInfo(const Circle& circle) const
	{
		switch (m_type)
		{
		case Types::Rectangle:
			return m_orientedRectangle.GetIntersectInfo(circle);
		case Types::Circle:
			return m_circle.GetIntersectInfo(circle);
			
		default:
			return { };
		}
	}
	
	float ColliderInfo::GetRayIntersectionDistance(glm::vec2 start, glm::vec2 direction) const
	{
		switch (m_type)
		{
		case Types::Circle:
			return m_circle.GetRayIntersectionDistance(start, direction);
		case Types::Rectangle:
			return m_orientedRectangle.GetRayIntersectionDistance(start, direction);
			
		default:
			return std::numeric_limits<float>::quiet_NaN();
		}
	}
}
