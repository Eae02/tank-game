#include "particlebatch.h"
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
	    : m_buffer(BUFFER_SIZE, GL_MAP_WRITE_BIT) { }
	
	void ParticleBatch::AddParticle(const Particle& particle, float timeInterpolation)
	{
		if (m_mappedMemory == nullptr)
			throw InvalidStateException("Memory not mapped when attempting to add a particle.");
		
		glm::mat4 worldMatrix(particle.GetWorldMatrix(timeInterpolation));
		reinterpret_cast<glm::mat4*>(m_mappedMemory)[m_numParticles] = worldMatrix;
		
		GetOpacitiesPtr()[m_numParticles] = glm::clamp(particle.GetOpacity(timeInterpolation), 0.0f, 1.0f);
		GetLayersPtr()[m_numParticles] = particle.GetTextureLayer();
		
		m_numParticles++;
	}
	
	void ParticleBatch::Begin()
	{
		if (m_mappedMemory != nullptr)
			return;
		m_mappedMemory = reinterpret_cast<char*>(glMapNamedBuffer(m_buffer.GetID(), GL_WRITE_ONLY));
		m_numParticles = 0;
	}
	
	void ParticleBatch::End()
	{
		if (m_mappedMemory == nullptr)
			return;
		glUnmapNamedBuffer(m_buffer.GetID());
		m_mappedMemory = nullptr;
	}
	
	float* ParticleBatch::GetOpacitiesPtr()
	{
		return reinterpret_cast<float*>(m_mappedMemory + OPACITY_BUFFER_OFFSET);
	}
	
	int32_t* ParticleBatch::GetLayersPtr()
	{
		return reinterpret_cast<int32_t*>(m_mappedMemory + LAYERS_BUFFER_OFFSET);
	}
	
	void ParticleBatch::Bind() const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_buffer.GetID());
	}
}
