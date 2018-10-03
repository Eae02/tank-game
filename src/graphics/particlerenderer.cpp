#include "particlerenderer.h"
#include "deferredrenderer.h"
#include "gl/shadermodule.h"
#include "quadmesh.h"
#include "../utils/ioutils.h"

namespace TankGame
{
	static ShaderProgram LoadShader()
	{
		auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "particle.vs.glsl", GL_VERTEX_SHADER);
		auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "particle.fs.glsl", GL_FRAGMENT_SHADER);
		
		return ShaderProgram({ &vs, &fs });
	}
	
	ParticleRenderer::ParticleRenderer()
	    : m_shader(LoadShader()) { }
	
	void ParticleRenderer::Begin(const Texture2D& lightBufferTexture)
	{
		m_usedBatches = 0;
		m_shader.Use();
		
		lightBufferTexture.Bind(0);
		
		QuadMesh::GetInstance().GetVAO().Bind();
		
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
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
