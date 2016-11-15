#include "particlesystementity.h"

#include <GLFW/glfw3.h>

namespace TankGame
{
	//The particle system reference cannot be used yet!
	ParticleSystemEntityBase::ParticleSystemEntityBase(IParticleSystem& particleSystem, double lifeTime)
	    : m_particleSystem(particleSystem), m_deathTime(glfwGetTime() + lifeTime)
	{
		
	}
}
