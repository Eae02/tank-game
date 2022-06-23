#include "particlerenderer.h"
#include "deferredrenderer.h"
#include "gl/shadermodule.h"
#include "quadmesh.h"
#include "../platform/common.h"
#include "../utils/ioutils.h"

namespace TankGame
{
	static ShaderProgram LoadShader()
	{
		ShaderProgram program(ShaderModule::FromResFile("particle.vs.glsl"), ShaderModule::FromResFile("particle.fs.glsl"));
		program.SetUniformBlockBinding("ParticlesUB", 1);
		program.SetTextureBinding("lightingSampler", 0);
		program.SetTextureBinding("diffuseSampler", 1);
		return program;
	}
	
	ParticleRenderer::ParticleRenderer()
	    : m_shader(LoadShader())
	{
		m_additiveBlendingUniformLocation = m_shader.GetUniformLocation("additiveBlend");
		m_aspectRatioUniformLocation = m_shader.GetUniformLocation("aspectRatio");
	}
	
	void ParticleRenderer::Begin(const Texture2D& lightBufferTexture)
	{
		m_usedBatches = 0;
		m_numRenderedParticles = 0;
		m_shader.Use();
		
		lightBufferTexture.Bind(0);
		
		QuadMesh::GetInstance().BindVAO();
		
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	}
	
	void ParticleRenderer::SetUniforms(bool additiveBlending, float textureAspectRatio)
	{
		glUniform1i(m_additiveBlendingUniformLocation, additiveBlending);
		glUniform1f(m_aspectRatioUniformLocation, textureAspectRatio);
	}
	
	ParticleBatch& ParticleRenderer::GetRenderBatch()
	{
		if (m_usedBatches >= m_particleBatches.size())
		{
			m_particleBatches.emplace_back(std::make_unique<ParticleBatch>());
		}
		
		ParticleBatch& batch = *m_particleBatches[m_usedBatches++];
		batch.Begin();
		return batch;
	}
	
	void ParticleRenderer::DrawBatch(ParticleBatch& batch)
	{
		batch.End();
		m_numRenderedParticles += batch.GetParticleCount();
		if (batch.GetParticleCount() != 0)
		{
			batch.Bind();
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, batch.GetParticleCount());
		}
	}
	
	void ParticleRenderer::End()
	{
		glDisable(GL_BLEND);
	}
}
