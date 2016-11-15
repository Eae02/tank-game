#pragma once

#include <glm/vec2.hpp>
#include "rectangle.h"
#include "world/intersectinfo.h"

namespace TankGame
{
	class Circle
	{
	public:
		inline Circle() { }
		
		inline Circle(glm::vec2 center, float radius)
		    : m_center(center), m_radius(radius) { }
		
		bool Intersects(const Circle& other) const;
		bool Contains(glm::vec2 point) const;
		
		inline glm::vec2 GetCenter() const
		{ return m_center; }
		inline float GetRadius() const
		{ return m_radius; }
		
		inline void SetCenter(glm::vec2 center)
		{ m_center = center; }
		inline void SetRadius(float radius)
		{ m_radius = radius; }
		
		inline Rectangle GetBoundingRectangle() const
		{
			return Rectangle::CreateCentered(m_center, m_radius * 2, m_radius * 2);
		}
		
		IntersectInfo GetIntersectInfo(const Circle& other) const;
		
	private:
		glm::vec2 m_center;
		float m_radius;
	};
}
