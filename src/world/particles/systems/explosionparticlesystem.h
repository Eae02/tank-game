#pragma once

#include "explosionflameemitter.h"
#include "sparkemitter.h"
#include "particlesystem.h"

namespace TankGame
{
	class ExplosionParticleSystem : public IParticleSystem
	{
	public:
		explicit ExplosionParticleSystem(ParticlesManager& particlesManager);
		
		virtual ParticleEmitter* GetEmitter(size_t n) override;
		
		virtual size_t GetEmitterCount() const override;
		
		virtual void SetTransformationProvider(const class ITransformationProvider* transformationProvider) override;
		
	private:
		ExplosionFlameEmitter m_flameEmitter;
		SparkEmitter m_sparkEmitter;
	};
}
