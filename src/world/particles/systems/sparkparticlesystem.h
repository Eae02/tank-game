#pragma once

#include "sparkemitter.h"
#include "particlesystem.h"
#include "../particleemitter.h"
#include "../spherevec2generator.h"
#include "../../../graphics/gl/texture2darray.h"

namespace TankGame
{
	class SparkParticleSystem : public IParticleSystem
	{
	public:
		explicit SparkParticleSystem(ParticlesManager& particlesManager);
		
		virtual ParticleEmitter* GetEmitter(size_t n) override;
		
		virtual size_t GetEmitterCount() const override;
		
		virtual void SetTransformationProvider(const class ITransformationProvider* transformationProvider) override;
		
	private:
		SparkEmitter m_emitter;
	};
}
