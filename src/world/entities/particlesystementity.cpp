#include "particlesystementity.h"
#include "../../platform/common.h"

namespace TankGame
{
	void ParticleSystemEntityBase::UpdateLastFrameTransform()
	{
		m_lastFrameTransform = GetTransform();
		m_hasLastFrameTransform = true;
	}
	
	//The particle system reference cannot be used yet!
	ParticleSystemEntityBase::ParticleSystemEntityBase(IParticleSystem& particleSystem, double lifeTime)
	    : m_particleSystem(particleSystem), m_deathTime(GetTime() + lifeTime) { }
}
