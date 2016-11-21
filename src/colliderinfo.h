#pragma once

#include "orientedrectangle.h"
#include "circle.h"

namespace TankGame
{
	class ColliderInfo
	{
	public:
		inline ColliderInfo() : m_type(Types::Null) { }
		
		inline ColliderInfo(const OrientedRectangle& rectangle)
		    : m_type(Types::Rectangle), m_orientedRectangle(rectangle) { }
		inline ColliderInfo(const Circle& circle)
		    : m_type(Types::Circle), m_circle(circle) { }
		
		IntersectInfo GetIntersectInfo(const Circle& circle) const;
		
		/*
			Returns the distance along the ray to the first intersection
			with the collider, or NaN if there is no intersection.
		*/
		float GetRayIntersectionDistance(glm::vec2 start, glm::vec2 direction) const;
		
	private:
		enum class Types
		{
			Null,
			Rectangle,
			Circle
		};
		
		Types m_type;
		
		OrientedRectangle m_orientedRectangle;
		Circle m_circle;
	};
}
