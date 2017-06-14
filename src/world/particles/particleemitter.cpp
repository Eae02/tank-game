#include "particleemitter.h"
#include "../../graphics/particlerenderer.h"

#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>

namespace TankGame
{
	std::mt19937 ParticleEmitter::s_random;
	
	ParticleEmitter::ParticleEmitter(ParticlesManager& particlesManager)
	    : m_emitterSettingsBuffer(BufferAllocator::GetInstance().AllocateUnique(8, GL_MAP_WRITE_BIT)),
	      m_particlesManager(particlesManager)
	{
		SetBeginOpacity(1.0f, 1.0f);
		SetEndOpacity(1.0f, 1.0f);
		SetBeginSize(1.0f, 1.0f);
		SetEndSize(1.0f, 1.0f);
		SetRotation(0.0f, glm::two_pi<float>());
		SetRotationSpeed(0.0f, 0.0f);
	}
	
	void ParticleEmitter::SpawnParticles(float elapsedTime)
	{
		elapsedTime += m_timeNotEmitted;
		
		int numEmissions = static_cast<int>(std::floor(elapsedTime * m_emissionRate));
		m_timeNotEmitted = elapsedTime - numEmissions / m_emissionRate;
		
		if (numEmissions > 0)
		{
			for (int i = 0; i < numEmissions; i++)
			{
				ParticleInfo info = GetParticleInfo((i + 1) / static_cast<float>(numEmissions));
				bool spawned = false;
				
				for (const ParticlePoolHandle& particlePool : m_particlePools)
				{
					if (particlePool->SpawnParticle(info))
					{
						spawned = true;
						break;
					}
				}
				
				if (!spawned)
				{
					m_particlePools.push_back(m_particlesManager.GetParticlePool());
					m_particlePools.back()->SpawnParticle(info);
				}
			}
		}
		
		for (size_t i = 0; i < m_particlePools.size();)
		{
			if (m_particlePools[i]->IsEmpty())
			{
				if (i < m_particlePools.size() - 1)
					m_particlePools[i] = std::move(m_particlePools.back());
				m_particlePools.pop_back();
			}
			else
				i++;
		}
	}
	
	ParticleInfo ParticleEmitter::GetParticleInfo(float subframeInterpolation)
	{
		ParticleInfo info;
		
		info.m_position = GeneratePosition(subframeInterpolation);
		info.m_velocity = GenerateVelocity(subframeInterpolation);
		
		info.m_rotation = m_rotationDist(s_random);
		info.m_rotationVelocity = m_rotationSpeedDist(s_random) * ((s_random() % 2) * 2 - 1);
		
		info.m_beginOpacity = m_beginOpacityDist(s_random);
		info.m_endOpacity = m_endOpacityDist(s_random);
		
		info.m_beginSize = m_beginSizeDist(s_random);
		info.m_endSize = m_endSizeDist(s_random);
		
		info.m_textureLayer = m_layerDist(s_random);
		info.m_lifeTime = m_lifeTimeDist(s_random);
		
		info.m_alignToVelocity = m_alignParticlesToVelocity;
		
		return info;
	}
	
	void ParticleEmitter::SetTextureArray(const Texture2DArray& textureArray)
	{
		m_textureArray = &textureArray;
		m_layerDist = std::uniform_int_distribution<int>(0, textureArray.GetLayers() - 1);
		m_settingsBufferOutOfDate = true;
	}
	
	void ParticleEmitter::SetUseAdditiveBlending(bool useAdditiveBlending)
	{
		m_useAdditiveBlending = useAdditiveBlending;
		m_settingsBufferOutOfDate = true;
	}
	
	bool ParticleEmitter::HasParticles() const
	{
		for (const ParticlePoolHandle& particlePool : m_particlePools)
		{
			if (!particlePool->IsEmpty())
				return true;
		}
		return false;
	}
	
	void ParticleEmitter::Render(ParticleRenderer& renderer) const
	{
		if (m_settingsBufferOutOfDate)
		{
			char* settingsMemory = reinterpret_cast<char*>(glMapNamedBuffer(*m_emitterSettingsBuffer, GL_WRITE_ONLY));
			
			*reinterpret_cast<float*>(settingsMemory) = m_textureArray->GetHeight() /
				static_cast<float>(m_textureArray->GetWidth());
			
			*reinterpret_cast<int32_t*>(settingsMemory + 4) = m_useAdditiveBlending ? 1 : 0;
			
			glUnmapNamedBuffer(*m_emitterSettingsBuffer);
			
			m_settingsBufferOutOfDate = false;
		}
		
		m_textureArray->Bind(1);
		
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, *m_emitterSettingsBuffer);
		
		ParticleBatch* batch = nullptr;
		
		double time = glfwGetTime();
		
		for (const ParticlePoolHandle& poolHandle : m_particlePools)
		{
			poolHandle->IterateParticles([&] (const Particle& particle)
			{
				if (batch == nullptr || batch->IsFull())
				{
					if (batch != nullptr)
						renderer.DrawBatch(*batch);
					batch = &renderer.GetRenderBatch();
				}
				
				batch->AddParticle(particle, particle.GetTimeInterpolation(time));
			});
		}
		
		if (batch != nullptr)
			renderer.DrawBatch(*batch);
	}
}
