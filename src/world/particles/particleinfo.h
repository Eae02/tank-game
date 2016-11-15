#pragma once

#include <glm/glm.hpp>

namespace TankGame
{
	struct ParticleInfo
	{
		glm::vec2 m_position;
		glm::vec2 m_velocity;
		float m_rotation;
		float m_rotationVelocity;
		
		float m_beginOpacity;
		float m_endOpacity;
		
		float m_beginSize;
		float m_endSize;
		
		int m_textureLayer;
		
		double m_lifeTime;
		
		bool m_alignToVelocity;
	};
}
