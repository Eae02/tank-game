#include "flamethrowerparticlesystem.h"

namespace TankGame
{
	FlameThrowerParticleSystem::FlameThrowerParticleSystem(float flameLength, ParticlesManager& particlesManager)
	    : m_flameEmitter(flameLength, particlesManager)
	{
		
	}
	
	ParticleEmitter* FlameThrowerParticleSystem::GetEmitter(size_t n)
	{
		return &m_flameEmitter;
	}
	
	size_t FlameThrowerParticleSystem::GetEmitterCount() const
	{
		return 1;
	}
	
	void FlameThrowerParticleSystem::SetTransformationProvider(const ITransformationProvider* transformationProvider)
	{
		m_flameEmitter.SetTransformationProvider(transformationProvider);
	}
}
