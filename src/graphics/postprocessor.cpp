#include "postprocessor.h"
#include "quadmesh.h"
#include "gl/shadermodule.h"
#include "gl/specializationinfo.h"
#include "../settings.h"
#include "../utils/ioutils.h"

#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace TankGame
{
	ShaderProgram PostProcessor::LoadShader(const fs::path& path)
	{
		ShaderModule fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / path, GL_FRAGMENT_SHADER);
		return ShaderProgram({ &QuadMesh::GetVertexShader(), &fs });
	}
	
	const int NOISE_TEXTURE_RES = 64;
	
	PostProcessor::PostProcessor()
	    : m_hexagonTexture(Texture2D::FromFile(GetResDirectory() / "hex.png")),
	      m_noiseTexture(NOISE_TEXTURE_RES, -1.0f, 1.0f),
	      m_bloomHBlurShader(LoadShader(fs::path("bloom") / "hblur.fs.glsl")),
	      m_bloomVBlurShader(LoadShader(fs::path("bloom") / "vblur.fs.glsl")),
	      m_postProcessShader(LoadShader("post.fs.glsl")),
	      m_blurVectorsBuffer(BufferAllocator::GetInstance().AllocateUnique(sizeof(float) * 4, GL_MAP_WRITE_BIT)),
	      m_postSettingsUB(BufferAllocator::GetInstance().AllocateUnique(sizeof(float) * 8, GL_MAP_WRITE_BIT))
	{
		m_hexagonTexture.SetWrapMode(GL_REPEAT);
	}
	
	void PostProcessor::DoPostProcessing(const Texture2D& inputTexture, const Texture2D& distortionsTexture) const
	{
		float damageFlashIntensity = 0;
		
		const double DAMAGE_FLASH_TIME = 0.5;
		
		double time = glfwGetTime();
		if (time < m_damageFlashBeginTime + DAMAGE_FLASH_TIME)
			damageFlashIntensity = (1.0f - (time - m_damageFlashBeginTime) / DAMAGE_FLASH_TIME) * 0.5f;
		
		if (m_postSettingsNeedUpload || damageFlashIntensity != m_oldDamageFlashIntensity)
		{
			void* memory = glMapNamedBufferRange(*m_postSettingsUB, 0, sizeof(float) * 8,
			                                     GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			
			reinterpret_cast<float*>(memory)[0] = m_exposure;
			reinterpret_cast<float*>(memory)[1] = m_gamma;
			reinterpret_cast<float*>(memory)[2] = m_contrast * (1.0f - damageFlashIntensity * 0.7f);
			reinterpret_cast<float*>(memory)[3] = m_framebufferAR;
			reinterpret_cast<float*>(memory)[4] = 0.75f;
			reinterpret_cast<float*>(memory)[5] = damageFlashIntensity;
			reinterpret_cast<float*>(memory)[6] = damageFlashIntensity * m_pixelWidth * 6;
			reinterpret_cast<float*>(memory)[7] = m_distortionSampleMul;
			
			glUnmapNamedBuffer(*m_postSettingsUB);
			
			m_postSettingsNeedUpload = false;
			m_oldDamageFlashIntensity = damageFlashIntensity;
		}
		
		QuadMesh::GetInstance().GetVAO().Bind();
		inputTexture.Bind(0);
		distortionsTexture.Bind(2);
		
		if (!m_bloomHBlurOutput.IsNull())
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, 1, *m_blurVectorsBuffer);
			
			// ** The horizontal bloom blur pass **
			Framebuffer::Save();
			Framebuffer::Bind(*m_bloomHBlurOutputFramebuffer, 0, 0, m_bloomHBlurOutput->GetWidth(),
			                  m_bloomHBlurOutput->GetHeight());
			
			m_bloomHBlurShader.Use();
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			
			
			// ** The vertical bloom blur pass **
			Framebuffer::Bind(*m_bloomVBlurOutputFramebuffer, 0, 0, m_bloomVBlurOutput->GetWidth(),
			                  m_bloomVBlurOutput->GetHeight());
			
			m_bloomHBlurOutput->Bind(1);
			m_bloomVBlurShader.Use();
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			
			m_bloomVBlurOutput->Bind(0);
			Framebuffer::Restore();
		}
		
		if (m_blurAmount > 1E-6)
		{
			Framebuffer::Save();
			Framebuffer::Bind(*m_blurInputFramebuffer, 0, 0, m_blurInputBuffer->GetWidth(),
			                  m_blurInputBuffer->GetHeight());
		}
		
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, *m_postSettingsUB);
		m_postProcessShader.Use();
		m_hexagonTexture.Bind(1);
		m_noiseTexture.Bind(3);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		if (m_blurAmount > 1E-6)
		{
			Framebuffer::Restore();
			m_blurPostProcess.DoBlurPass(m_blurAmount);
		}
	}
	
	void PostProcessor::DoDamageFlash()
	{
		m_damageFlashBeginTime = glfwGetTime();
	}
	
	void PostProcessor::SetBlurAmount(float blurAmount)
	{
		m_blurAmount = glm::clamp(blurAmount, 0.0f, 1.0f);
	}
	
	void PostProcessor::SetGamma(float gamma)
	{
		m_gamma = gamma;
		m_postSettingsNeedUpload = true;
	}
	
	void PostProcessor::OnResize(GLsizei newWidth, GLsizei newHeight)
	{
		m_blurInputFramebuffer.Construct();
		m_blurInputBuffer.Construct(newWidth, newHeight, 1, GL_RGBA8);
		glNamedFramebufferTexture(m_blurInputFramebuffer->GetID(), GL_COLOR_ATTACHMENT0, m_blurInputBuffer->GetID(), 0);
		glNamedFramebufferDrawBuffer(m_blurInputFramebuffer->GetID(), GL_COLOR_ATTACHMENT0);
		m_blurInputBuffer->SetWrapMode(GL_CLAMP_TO_EDGE);
		
		m_blurPostProcess.CreateFramebuffer(*m_blurInputBuffer);
		
		if (!Settings::GetInstance().EnableBloom())
		{
			m_bloomHBlurOutput.Destroy();
			m_bloomHBlurOutputFramebuffer.Destroy();
			
			m_bloomVBlurOutput.Destroy();
			m_bloomVBlurOutputFramebuffer.Destroy();
			
			return;
		}
		
		m_distortionSampleMul = 4.0f * static_cast<float>(newHeight) / static_cast<float>(NOISE_TEXTURE_RES);
		m_framebufferAR = static_cast<float>(newWidth) / static_cast<float>(newHeight);
		m_pixelWidth = 1.0f / newWidth;
		m_postSettingsNeedUpload = true;
		
		GLsizei hBlurWidth = newWidth / 2;
		GLsizei hBlurHeight = newHeight / 2;
		
		bool uploadBlurVectors = false;
		
		if (m_bloomHBlurOutput.IsNull() || hBlurWidth != m_bloomHBlurOutput->GetWidth() ||
			hBlurHeight != m_bloomHBlurOutput->GetHeight())
		{
			m_bloomHBlurOutput.Construct(hBlurWidth, hBlurHeight, 1, GL_RGB16F);
			m_bloomHBlurOutput->SetWrapMode(GL_CLAMP_TO_EDGE);
			m_bloomHBlurOutput->SetMagFilter(GL_LINEAR);
			m_bloomHBlurOutput->SetMinFilter(GL_LINEAR);
			
			m_bloomHBlurOutputFramebuffer.Construct();
			glNamedFramebufferTexture(m_bloomHBlurOutputFramebuffer->GetID(), GL_COLOR_ATTACHMENT0,
			                          m_bloomHBlurOutput->GetID(), 0);
			glNamedFramebufferDrawBuffer(m_bloomHBlurOutputFramebuffer->GetID(), GL_COLOR_ATTACHMENT0);
			
			uploadBlurVectors = true;
		}
		
		if (m_bloomVBlurOutput.IsNull() || newWidth != m_bloomVBlurOutput->GetWidth() ||
			newHeight != m_bloomVBlurOutput->GetHeight())
		{
			m_bloomVBlurOutput.Construct(newWidth, newHeight, 1, GL_RGB16F);
			m_bloomVBlurOutput->SetWrapMode(GL_CLAMP_TO_EDGE);
			m_bloomVBlurOutput->SetMagFilter(GL_LINEAR);
			m_bloomVBlurOutput->SetMinFilter(GL_LINEAR);
			
			m_bloomVBlurOutputFramebuffer.Construct();
			glNamedFramebufferTexture(m_bloomVBlurOutputFramebuffer->GetID(), GL_COLOR_ATTACHMENT0,
			                          m_bloomVBlurOutput->GetID(), 0);
			glNamedFramebufferDrawBuffer(m_bloomVBlurOutputFramebuffer->GetID(), GL_COLOR_ATTACHMENT0);
			
			uploadBlurVectors = true;
		}
		
		if (uploadBlurVectors)
		{
			glm::vec2* blurVectorsMemory = reinterpret_cast<glm::vec2*>(
				glMapNamedBufferRange(*m_blurVectorsBuffer, 0, sizeof(float) * 4,
				                      GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
			
			blurVectorsMemory[0] = { 2.0f / static_cast<float>(newWidth), 0.0f };
			blurVectorsMemory[1] = { 0.0f, 2.0f / static_cast<float>(newHeight) };
			
			glUnmapNamedBuffer(*m_blurVectorsBuffer);
		}
	}
}
