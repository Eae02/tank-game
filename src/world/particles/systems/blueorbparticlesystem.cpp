#include "blueorbparticlesystem.h"

namespace TankGame
{
	BlueOrbParticleSystem::BlueOrbParticleSystem(ParticlesManager& particlesManager)
	    : m_blueOrbEmitter(particlesManager)
	{
		
	}
	
	ParticleEmitter* BlueOrbParticleSystem::GetEmitter(size_t n)
	{
		return &m_blueOrbEmitter;
	}
	
	size_t BlueOrbParticleSystem::GetEmitterCount() const
	{
		return 1;
	}
	
	void BlueOrbParticleSystem::SetTransformationProvider(const ITransformationProvider* transformationProvider)
	{
		m_blueOrbEmitter.SetTransformationProvider(transformationProvider);
	}
}
