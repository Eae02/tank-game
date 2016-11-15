#pragma once

#include "../../utils/memory/objectpool.h"
#include "particle.h"
#include <array>

namespace TankGame
{
	class ParticlePool
	{
	public:
		ParticlePool();
		
		bool SpawnParticle(const ParticleInfo& info);
		
		void Update(float dt);
		
		bool IsEmpty() const
		{ return m_numParticles == 0; }
		inline size_t GetParticleCount() const
		{ return m_numParticles; }
		
		template <typename CallbackTp>
		void IterateParticles(CallbackTp callback) const
		{
			for (size_t i = 0; i < m_numParticles; i++)
				callback(m_particles[i]);
		}
		
	private:
		std::array<Particle, 1024> m_particles;
		size_t m_numParticles = 0;
	};
}
