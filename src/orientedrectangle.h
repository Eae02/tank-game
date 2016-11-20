#pragma once

#include <glm/glm.hpp>
#include "world/intersectinfo.h"
#include "transform.h"

namespace TankGame
{
	class OrientedRectangle
	{
	public:
		OrientedRectangle();
		
		OrientedRectangle(glm::vec2 center, glm::vec2 size, float rotation);
		
		static OrientedRectangle FromTransformedNDC(const class Transform& transform)
		{
			return { transform.GetPosition(), transform.GetScale(), transform.GetRotation() };
		}
		
		IntersectInfo GetIntersectInfo(const class Circle& circle) const;
		
		bool Contains(glm::vec2 point) const;
		
	private:
		glm::vec2 m_edgeNormals[4];
		glm::vec2 m_edgePositions[4];
	};
}
