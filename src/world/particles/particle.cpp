#include "particle.h"
#include "../../platform/common.h"

namespace TankGame
{
	Particle::Particle(const ParticleInfo& particleInfo)
	    : ParticleInfo(particleInfo), m_spawnTime(GetTime()) { }
	
	void Particle::Update(float dt)
	{
		m_position += m_velocity * dt;
		m_rotation += m_rotationVelocity * dt;
	}
	
	glm::mat3 Particle::GetWorldMatrix(float timeInterpolation) const
	{
		glm::mat3 R;
		
		if (m_alignToVelocity)
		{
			glm::vec2 normV = glm::normalize(m_velocity);
			
			R = glm::mat3(
				normV.x, normV.y, 0.0f,
				normV.y, -normV.x, 0.0f,
				0.0f, 0.0f, 1.0f
			);
		}
		else
		{
			float cosR = std::cos(m_rotation);
			float sinR = std::sin(m_rotation);
			
			R = glm::mat3(
				cosR, sinR, 0,
				-sinR, cosR, 0,
				0, 0, 1
			);
		}
		
		glm::mat3 T = glm::mat3(
			1, 0, 0,
			0, 1, 0,
			m_position.x, m_position.y, 1
		);
		
		float size = GetSize(timeInterpolation);
		glm::mat3 S(
			size, 0, 0,
			0, size, 0,
			0, 0,    1
		);
		
		return T * R * S;
	}
}
