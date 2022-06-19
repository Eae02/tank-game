#include "particlepool.h"
#include "../../utils/utils.h"
#include "../../platform/common.h"

#include <iostream>

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
		double time = GetTime();
		
		for (size_t i = 0; i < m_numParticles;)
		{
			if (time >= m_particles[i].GetDeathTime())
			{
				m_numParticles--;
				m_particles[i] = m_particles[m_numParticles];
			}
			else
			{
				m_particles[i].Update(dt);
				i++;
			}
		}
	}
}
