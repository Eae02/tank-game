#pragma once

#include "blueorbemitter.h"
#include "particlesystem.h"

namespace TankGame
{
	class BlueOrbParticleSystem : public IParticleSystem
	{
	public:
		explicit BlueOrbParticleSystem(ParticlesManager& particlesManager);
		
		virtual ParticleEmitter* GetEmitter(size_t n) override;
		
		virtual size_t GetEmitterCount() const override;
		
		virtual void SetTransformationProvider(const class ITransformationProvider* transformationProvider) override;
		
	private:
		BlueOrbEmitter m_blueOrbEmitter;
	};
}
