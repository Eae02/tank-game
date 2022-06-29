#include "blurpostprocess.h"
#include "gl/shadermodule.h"
#include "quadmesh.h"
#include "../utils/ioutils.h"


namespace TankGame
{
	static ShaderProgram LoadShader()
	{
		ShaderProgram program(QuadMesh::GetVertexShader(), ShaderModule::FromResFile("postblur.fs.glsl"));
		program.SetTextureBinding("inputSampler", 0);
		return program;
	}
	
	BlurPostProcess::BlurPostProcess()
	    : m_shader(LoadShader()),
		  m_blurVectorUniformLocation(m_shader.GetUniformLocation("blurVector")),
		  m_sampleOffsetUniformLocation(m_shader.GetUniformLocation("sampleOffset"))
	{
		
	}
	
	void BlurPostProcess::CreateFramebuffer(const Texture2D& inputTexture)
	{
		m_input = &inputTexture;
		
		m_framebuffer = std::make_unique<Framebuffer>();
		
		m_intermidiateBuffer = std::make_unique<Texture2D>(inputTexture.GetWidth(), inputTexture.GetHeight(), 1, TextureFormat::RGBA16F);
		glNamedFramebufferTexture(m_framebuffer->GetID(), GL_COLOR_ATTACHMENT0, m_intermidiateBuffer->GetID(), 0);
		glNamedFramebufferDrawBuffer(m_framebuffer->GetID(), GL_COLOR_ATTACHMENT0);
		
		m_intermidiateBuffer->SetWrapMode(GL_CLAMP_TO_EDGE);
		m_intermidiateBuffer->SetMagFilter(GL_LINEAR);
		m_intermidiateBuffer->SetMinFilter(GL_LINEAR);
	}
	
	void BlurPostProcess::DoBlurPass(float blurAmount) const
	{
		if (m_input == nullptr || blurAmount < 1E-6)
			return;
		
		float blurAmount2 = blurAmount * 2.0f;
		glm::vec2 blurVector(blurAmount2 / m_input->GetWidth(), blurAmount2 / m_input->GetHeight());
		glm::vec2 sampleOffset(0.5f / m_input->GetWidth(), 0.5f / m_input->GetHeight());
		
		QuadMesh::GetInstance().BindVAO();
		
		Framebuffer::Save();
		Framebuffer::Bind(*m_framebuffer, 0, 0, m_input->GetWidth(), m_input->GetHeight());
		
		m_shader.Use();
		
		m_input->Bind(0);
		
		glUniform2f(m_blurVectorUniformLocation, blurVector.x, 0.0f);
		glUniform2f(m_sampleOffsetUniformLocation, sampleOffset.x, sampleOffset.y);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		Framebuffer::Restore();
		m_intermidiateBuffer->Bind(0);
		
		glUniform2f(m_blurVectorUniformLocation, 0.0f, blurVector.y);
		glUniform2f(m_sampleOffsetUniformLocation, -sampleOffset.x, -sampleOffset.y);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}
