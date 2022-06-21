#pragma once

#include <vector>
#include <memory>
#include "particlebatch.h"
#include "gl/shaderprogram.h"

namespace TankGame
{
	class ParticleRenderer
	{
	public:
		ParticleRenderer();
		
		void Begin(const class Texture2D& lightBufferTexture);
		void End();
		
		ParticleBatch& GetRenderBatch();
		
		void DrawBatch(ParticleBatch& batch);
		
		void SetUniforms(bool additiveBlending, float textureAspectRatio);
		
		uint32_t GetNumRenderedParticles() const { return m_numRenderedParticles; }
		
		bool measureDrawCPUTime = false;
		double LastDrawCPUTime() const { return m_lastDrawCPUTime; }
		
	private:
		std::vector<std::unique_ptr<ParticleBatch>> m_particleBatches;
		size_t m_usedBatches = 0;
		uint32_t m_numRenderedParticles = 0;
		
		ShaderProgram m_shader;
		int m_additiveBlendingUniformLocation;
		int m_aspectRatioUniformLocation;
		
		double m_drawBeginTime = 0;
		double m_lastDrawCPUTime = 0;
	};
}
