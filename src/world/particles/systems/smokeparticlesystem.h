#pragma once

#include "smokeemitter.h"
#include "particlesystem.h"

namespace TankGame
{
	class SmokeParticleSystem : public IParticleSystem
	{
	public:
		explicit SmokeParticleSystem(ParticlesManager& particlesManager);
		
		virtual ParticleEmitter* GetEmitter(size_t n) override;
		virtual size_t GetEmitterCount() const override;
		
		virtual void SetTransformationProvider(const ITransformationProvider* transformationProvider) override;
		
	private:
		SmokeEmitter m_emitter;
	};
}
