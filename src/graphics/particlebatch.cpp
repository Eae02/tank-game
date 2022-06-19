#include "particlebatch.h"
#include "frames.h"
#include "../world/particles/particle.h"
#include "../exceptions/invalidstateexception.h"

#include <iostream>
#include <cstring>

namespace TankGame
{
	constexpr int ParticleBatch::BATCH_SIZE;
	constexpr size_t ParticleBatch::OPACITY_BUFFER_OFFSET;
	constexpr size_t ParticleBatch::LAYERS_BUFFER_OFFSET;
	constexpr size_t ParticleBatch::BUFFER_SIZE;
	
	ParticleBatch::ParticleBatch()
	    : m_buffer(BUFFER_SIZE * MAX_QUEUED_FRAMES, BufferUsage::MapWritePersistent)
	{
		for (uint32_t i = 0; i < MAX_QUEUED_FRAMES; i++)
		{
			m_bufferMemory[i] = m_buffer.MappedMemory() + BUFFER_SIZE * i;
		}
	}
	
	void ParticleBatch::AddParticle(const Particle& particle, float timeInterpolation)
	{
		glm::mat4 worldMatrix(particle.GetWorldMatrix(timeInterpolation));
		reinterpret_cast<glm::mat4*>(m_currentFrameBufferMemory)[m_numParticles] = worldMatrix;
		
		GetOpacitiesPtr()[m_numParticles] = glm::clamp(particle.GetOpacity(timeInterpolation), 0.0f, 1.0f);
		GetLayersPtr()[m_numParticles] = particle.GetTextureLayer();
		
		m_numParticles++;
	}
	
	void ParticleBatch::Begin()
	{
		m_numParticles = 0;
		m_currentFrameBufferMemory = m_bufferMemory[GetFrameQueueIndex()];
	}
	
	void ParticleBatch::End()
	{
		m_buffer.FlushMappedMemory(BUFFER_SIZE * GetFrameQueueIndex(), BUFFER_SIZE);
	}
	
	float* ParticleBatch::GetOpacitiesPtr()
	{
		return reinterpret_cast<float*>(m_currentFrameBufferMemory + OPACITY_BUFFER_OFFSET);
	}
	
	int32_t* ParticleBatch::GetLayersPtr()
	{
		return reinterpret_cast<int32_t*>(m_currentFrameBufferMemory + LAYERS_BUFFER_OFFSET);
	}
	
	void ParticleBatch::Bind() const
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_buffer.GetID(), BUFFER_SIZE * GetFrameQueueIndex(), BUFFER_SIZE);
	}
}
