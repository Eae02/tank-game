#include "particlesmanager.h"
#include "../../platform/common.h"
#include "../../profiling.h"

#include <algorithm>

namespace TankGame
{
	ParticlePoolHandle ParticlesManager::GetParticlePool()
	{
		ParticlePool* particlePool;
		
		if (!m_freeParticlePools.empty())
		{
			particlePool = m_freeParticlePools.back().get();
			
			m_activeParticlePools.emplace_back(std::move(m_freeParticlePools.back()));
			m_freeParticlePools.pop_back();
			
			*particlePool = ParticlePool();
		}
		else
		{
			m_activeParticlePools.emplace_back(std::make_unique<ParticlePool>());
			particlePool = m_activeParticlePools.back().get();
		}
		
		return ParticlePoolHandle(particlePool, ParticlePoolDeleter(*this));
	}
	
	void ParticlesManager::ReleaseParticlePool(const ParticlePool* pool)
	{
		auto pos = std::find_if(m_activeParticlePools.begin(), m_activeParticlePools.end(),
		                        [pool] (const std::unique_ptr<ParticlePool>& p) { return p.get() == pool; });
		if (pos == m_activeParticlePools.end())
			return;
		
		m_freeParticlePools.emplace_back(std::move(*pos));
		
		if (pos != m_activeParticlePools.begin() + m_activeParticlePools.size() - 1)
			*pos = std::move(m_activeParticlePools.back());
		m_activeParticlePools.pop_back();
	}
	
	void ParticlesManager::Update(float dt)
	{
		FUNC_TIMER
		for (const std::unique_ptr<ParticlePool>& pool : m_activeParticlePools)
		{
			pool->Update(dt);
		}
	}
	
	long ParticlesManager::GetParticleCount() const
	{
		long particleCount = 0;
		
		for (const std::unique_ptr<ParticlePool>& pool : m_activeParticlePools)
			particleCount += pool->GetParticleCount();
		
		return particleCount;
	}
}
