#include "energyballparticlesystem.h"

namespace TankGame
{
	EnergyBallParticleSystem::EnergyBallParticleSystem(ParticlesManager& particlesManager)
	     : m_sparkEmitter(particlesManager, glm::pi<float>(), 0.2f),
	       m_plasmaEmitter(particlesManager)
	{
		
	}
	
	ParticleEmitter* EnergyBallParticleSystem::GetEmitter(size_t n)
	{
		switch (n)
		{
		case 0: return &m_sparkEmitter;
		case 1: return &m_plasmaEmitter;
		}
		
		return nullptr;
	}
	
	size_t EnergyBallParticleSystem::GetEmitterCount() const
	{
		return 2;
	}
	
	void EnergyBallParticleSystem::SetTransformationProvider(const class ITransformationProvider* transformationProvider)
	{
		m_sparkEmitter.SetTransformationProvider(transformationProvider);
		m_plasmaEmitter.SetTransformationProvider(transformationProvider);
	}
}
