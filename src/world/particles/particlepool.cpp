#include "particlepool.h"
#include "../../utils/utils.h"
#include "../../platform/common.h"

namespace TankGame
{
	bool ParticlePool::SpawnParticle(const ParticleInfo& info)
	{
		if (m_numParticles >= m_particles.size())
			return false;
		m_particles[m_numParticles++] = Particle(info);
		return true;
	}
	
	void ParticlePool::Update(float dt)
	{
		glm::vec2 minBounds(INFINITY);
		glm::vec2 maxBounds(-INFINITY);
		
		for (size_t i = 0; i < m_numParticles;)
		{
			if (frameBeginTime >= m_particles[i].GetDeathTime())
			{
				m_numParticles--;
				m_particles[i] = m_particles[m_numParticles];
			}
			else
			{
				m_particles[i].Update(dt);
				
				minBounds = glm::min(minBounds, m_particles[i].GetPosition() - m_particles[i].GetBoundingSize());
				maxBounds = glm::max(maxBounds, m_particles[i].GetPosition() + m_particles[i].GetBoundingSize());
				
				i++;
			}
		}
		
		m_bounds = Rectangle(
			minBounds.x, minBounds.y,
			std::max(maxBounds.x - minBounds.x, 0.0f),
			std::max(maxBounds.y - minBounds.y, 0.0f)
		);
	}
}
