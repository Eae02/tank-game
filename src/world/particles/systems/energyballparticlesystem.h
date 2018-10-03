#pragma once

#include "sparkemitter.h"
#include "energyplasmaemitter.h"
#include "particlesystem.h"

namespace TankGame
{
	class EnergyBallParticleSystem : public IParticleSystem
	{
	public:
		explicit EnergyBallParticleSystem(ParticlesManager& particlesManager);
		
		virtual ParticleEmitter*GetEmitter(size_t n) override;
		virtual size_t GetEmitterCount() const override;
		virtual void SetTransformationProvider(const ITransformationProvider* transformationProvider) override;
		
	private:
		SparkEmitter m_sparkEmitter;
		EnergyPlasmaEmitter m_plasmaEmitter;
	};
}
