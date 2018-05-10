#include "blurpostprocess.h"
#include "gl/shadermodule.h"
#include "quadmesh.h"
#include "../utils/ioutils.h"

#include <glm/glm.hpp>

namespace TankGame
{
	static ShaderProgram LoadShader()
	{
		auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "postblur.fs.glsl", GL_FRAGMENT_SHADER);
		
		return ShaderProgram({ &QuadMesh::GetVertexShader(), &fs });
	}
	
	BlurPostProcess::BlurPostProcess()
	    : m_shader(LoadShader()), m_blurVectorUniformLocation(m_shader.GetUniformLocation("blurVector"))
	{
		
	}
	
	void BlurPostProcess::CreateFramebuffer(const Texture2D& inputTexture)
	{
		m_input = &inputTexture;
		
		m_framebuffer = std::make_unique<Framebuffer>();
		
		m_intermidiateBuffer = std::make_unique<Texture2D>(inputTexture.GetWidth(), inputTexture.GetHeight(), 1, GL_RGBA16F);
		glNamedFramebufferTexture(m_framebuffer->GetID(), GL_COLOR_ATTACHMENT0, m_intermidiateBuffer->GetID(), 0);
		glNamedFramebufferDrawBuffer(m_framebuffer->GetID(), GL_COLOR_ATTACHMENT0);
		
		m_intermidiateBuffer->SetWrapMode(GL_CLAMP_TO_EDGE);
	}
	
	void BlurPostProcess::DoBlurPass(float blurAmount) const
	{
		if (m_input == nullptr || blurAmount < 1E-6)
			return;
		
		glm::vec2 blurVector(blurAmount / m_input->GetWidth(), blurAmount / m_input->GetHeight());
		
		QuadMesh::GetInstance().GetVAO().Bind();
		
		Framebuffer::Save();
		Framebuffer::Bind(*m_framebuffer, 0, 0, m_input->GetWidth(), m_input->GetHeight());
		
		m_shader.Use();
		
		m_input->Bind(0);
		
		glUniform2f(m_blurVectorUniformLocation, blurVector.x, 0.0f);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		Framebuffer::Restore();
		m_intermidiateBuffer->Bind(0);
		
		glUniform2f(m_blurVectorUniformLocation, 0.0f, blurVector.y);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
}
