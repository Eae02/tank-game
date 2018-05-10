#pragma once

#include "directionalflameemitter.h"
#include "../particlesystem.h"

namespace TankGame
{
	class FlameThrowerParticleSystem : public IParticleSystem
	{
	public:
		FlameThrowerParticleSystem(float flameLength, class ParticlesManager& particlesManager);
		
		virtual ParticleEmitter* GetEmitter(size_t n) override;
		virtual size_t GetEmitterCount() const override;
		virtual void SetTransformationProvider(const ITransformationProvider* transformationProvider) override;
		
	private:
		DirectionalFlameEmitter m_flameEmitter;
	};
}
