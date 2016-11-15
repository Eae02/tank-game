#pragma once

#include "gl/buffer.h"
#include <cstdint>

namespace TankGame
{
	class ParticleBatch
	{
	public:
		ParticleBatch();
		
		void Begin();
		
		void AddParticle(const class Particle& particle, float timeInterpolation);
		
		void End();
		
		void Bind() const;
		
		static constexpr int BATCH_SIZE = 256;
		
		inline bool IsFull()
		{ return m_numParticles >= BATCH_SIZE; }
		inline size_t GetParticleCount() const
		{ return m_numParticles; }
		
	private:
		float* GetOpacitiesPtr();
		int32_t* GetLayersPtr();
		
		static constexpr size_t OPACITY_BUFFER_OFFSET = sizeof(float) * 4 * 4 * BATCH_SIZE;
		static constexpr size_t LAYERS_BUFFER_OFFSET = sizeof(float) * BATCH_SIZE + OPACITY_BUFFER_OFFSET;
		static constexpr size_t BUFFER_SIZE = sizeof(int32_t) * BATCH_SIZE + LAYERS_BUFFER_OFFSET;
		
		size_t m_numParticles;
		
		Buffer m_buffer;
		
		char* m_mappedMemory = nullptr;
	};
}
