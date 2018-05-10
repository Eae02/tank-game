#include "particlepool.h"
#include "../../utils/utils.h"

#include <GLFW/glfw3.h>
#include <iostream>

namespace TankGame
{
	ParticlePool::ParticlePool()
	{
		
	}
	
	bool ParticlePool::SpawnParticle(const ParticleInfo& info)
	{
		if (m_numParticles >= m_particles.size())
			return false;
		m_particles[m_numParticles++] = Particle(info);
		return true;
	}
	
	void ParticlePool::Update(float dt)
	{
		double time = glfwGetTime();
		
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
