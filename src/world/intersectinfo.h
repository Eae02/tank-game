#pragma once

#include <glm/vec2.hpp>

namespace TankGame
{
	struct IntersectInfo
	{
		inline IntersectInfo() : m_intersects(false) { }
		
		glm::vec2 m_penetration;
		bool m_intersects;
	};
}
