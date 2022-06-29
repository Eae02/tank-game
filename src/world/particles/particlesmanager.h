#pragma once

#include "particlepool.h"

namespace TankGame
{
	class ParticlesManager
	{
	public:
		class ParticlePoolDeleter
		{
		public:
			inline explicit ParticlePoolDeleter(ParticlesManager& manager)
			    : m_manager(&manager) { }
			
			inline void operator()(ParticlePool* pool)
			{ m_manager->ReleaseParticlePool(pool); }
		private:
			ParticlesManager* m_manager;
		};
		
		using ParticlePoolHandle = std::unique_ptr<ParticlePool, ParticlesManager::ParticlePoolDeleter>;
		
		ParticlesManager() = default;
		
		ParticlesManager(ParticlesManager&& other) = delete;
		ParticlesManager& operator=(ParticlesManager&& other) = delete;
		ParticlesManager(const ParticlesManager& other) = delete;
		ParticlesManager& operator=(const ParticlesManager& other) = delete;
		
		ParticlePoolHandle GetParticlePool();
		void ReleaseParticlePool(const ParticlePool* pool);
		
		void Update(float dt);
		
		long GetParticleCount() const;
		
	private:
		std::vector<std::unique_ptr<ParticlePool>> m_activeParticlePools;
		std::vector<std::unique_ptr<ParticlePool>> m_freeParticlePools;
	};
	
	using ParticlePoolHandle = ParticlesManager::ParticlePoolHandle;
}
