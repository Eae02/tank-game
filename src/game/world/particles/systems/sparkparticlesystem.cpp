#include "sparkparticlesystem.h"
#include "../../../utils/ioutils.h"
#include "../../../utils/utils.h"

#include <glm/gtc/constants.hpp>

namespace TankGame
{
	SparkParticleSystem::SparkParticleSystem(ParticlesManager& particlesManager)
	     : m_emitter(particlesManager, glm::radians(70.0f))
	{
		
	}
	
	ParticleEmitter* SparkParticleSystem::GetEmitter(size_t n)
	{
		return &m_emitter;
	}
	
	size_t SparkParticleSystem::GetEmitterCount() const
	{
		return 1;
	}
	
	void SparkParticleSystem::SetTransformationProvider(const class ITransformationProvider* transformationProvider)
	{
		m_emitter.SetTransformationProvider(transformationProvider);
	}
}
