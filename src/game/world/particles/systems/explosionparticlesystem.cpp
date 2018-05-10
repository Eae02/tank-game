#include "explosionparticlesystem.h"
#include "../../../utils/ioutils.h"
#include "../../../utils/utils.h"

#include <glm/gtc/constants.hpp>

namespace TankGame
{
	ExplosionParticleSystem::ExplosionParticleSystem(ParticlesManager& particlesManager)
	     : m_flameEmitter(particlesManager), m_sparkEmitter(particlesManager, glm::pi<float>(), 2)
	{
		
	}
	
	ParticleEmitter* ExplosionParticleSystem::GetEmitter(size_t n)
	{
		switch (n)
		{
		case 0: return &m_flameEmitter;
		case 1: return &m_sparkEmitter;
		}
		
		return nullptr;
	}
	
	size_t ExplosionParticleSystem::GetEmitterCount() const
	{
		return 2;
	}
	
	void ExplosionParticleSystem::SetTransformationProvider(const class ITransformationProvider* transformationProvider)
	{
		m_flameEmitter.SetTransformationProvider(transformationProvider);
		m_sparkEmitter.SetTransformationProvider(transformationProvider);
	}
}
