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
		
	private:
		std::vector<std::unique_ptr<ParticleBatch>> m_particleBatches;
		size_t m_usedBatches;
		
		ShaderProgram m_shader;
		int m_additiveBlendingUniformLocation;
		int m_aspectRatioUniformLocation;
	};
}
