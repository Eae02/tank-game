#include "smokeparticlesystem.h"

namespace TankGame
{
	SmokeParticleSystem::SmokeParticleSystem(ParticlesManager& particlesManager)
	    : m_emitter(particlesManager)
	{
		
	}
	
	ParticleEmitter* SmokeParticleSystem::GetEmitter(size_t n)
	{
		return &m_emitter;
	}
	
	size_t SmokeParticleSystem::GetEmitterCount() const
	{
		return 1;
	}
	
	void SmokeParticleSystem::SetTransformationProvider(const ITransformationProvider* transformationProvider)
	{
		m_emitter.SetTransformationProvider(transformationProvider);
	}
}
