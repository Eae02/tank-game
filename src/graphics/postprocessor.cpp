#include "postprocessor.h"
#include "quadmesh.h"
#include "gl/shadermodule.h"
#include "gl/specializationinfo.h"
#include "../settings.h"
#include "../platform/common.h"
#include "../utils/ioutils.h"
#include "../utils/utils.h"

#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace TankGame
{
	ShaderProgram PostProcessor::LoadShader(const fs::path& path)
	{
		ShaderModule fs = ShaderModule::FromFile(resDirectoryPath / "shaders" / path, GL_FRAGMENT_SHADER);
		return ShaderProgram({ &QuadMesh::GetVertexShader(), &fs });
	}
	
	const int NOISE_TEXTURE_RES = 64;
	
	PostProcessor::PostProcessor()
	    : m_hexagonTexture(Texture2D::FromFile(resDirectoryPath / "hex.png")),
	      m_noiseTexture(NOISE_TEXTURE_RES, -1.0f, 1.0f),
	      m_bloomHBlurShader(LoadShader(fs::path("bloom") / "hblur.fs.glsl")),
	      m_bloomVBlurShader(LoadShader(fs::path("bloom") / "vblur.fs.glsl")),
	      m_postProcessShader(LoadShader("post.fs.glsl"))
	{
		m_hexagonTexture.SetWrapMode(GL_REPEAT);
		
		m_postProcessShader.SetTextureBinding("inputSampler", 0);
		m_postProcessShader.SetTextureBinding("hexagonSampler", 1);
		m_postProcessShader.SetTextureBinding("distortionSampler", 2);
		m_postProcessShader.SetTextureBinding("noiseSampler", 3);
		
		m_exposureUniformLoc                   = m_postProcessShader.GetUniformLocation("exposure");
		m_gammaUniformLoc                      = m_postProcessShader.GetUniformLocation("gamma");
		m_contrastUniformLoc                   = m_postProcessShader.GetUniformLocation("contrast");
		m_framebufferARUniformLoc              = m_postProcessShader.GetUniformLocation("framebufferAR");
		m_damageFlashIntensityUniformLoc       = m_postProcessShader.GetUniformLocation("damageFlashIntensity");
		m_horizontalDistortionAmountUniformLoc = m_postProcessShader.GetUniformLocation("horizontalDistortionAmount");
		m_distortionSampleMulUniformLoc        = m_postProcessShader.GetUniformLocation("distortionSampleMul");
		
		m_bloomVBlurUniformLocations.blurVector = m_bloomVBlurShader.GetUniformLocation("blurVector");
		m_bloomVBlurUniformLocations.texCoordOffset = m_bloomVBlurShader.GetUniformLocation("texCoordOffset");
		m_bloomVBlurShader.SetTextureBinding("inputSampler", 0);
		m_bloomVBlurShader.SetTextureBinding("bloomInputSampler", 1);
		
		m_bloomHBlurUniformLocations.blurVector = m_bloomHBlurShader.GetUniformLocation("blurVector");
		m_bloomHBlurUniformLocations.texCoordOffset = m_bloomHBlurShader.GetUniformLocation("texCoordOffset");
		m_bloomHBlurShader.SetTextureBinding("inputSampler", 0);
	}
	
	void PostProcessor::DoPostProcessing(const Texture2D& inputTexture, const Texture2D& distortionsTexture) const
	{
		float damageFlashIntensity = 0;
		
		const double DAMAGE_FLASH_TIME = 0.5;
		
		if (frameBeginTime < m_damageFlashBeginTime + DAMAGE_FLASH_TIME)
			damageFlashIntensity = (1.0f - (frameBeginTime - m_damageFlashBeginTime) / DAMAGE_FLASH_TIME) * 0.5f;
		
		QuadMesh::GetInstance().BindVAO();
		inputTexture.Bind(0);
		distortionsTexture.Bind(2);
		
		if (m_bloomHBlurOutput != nullptr)
		{
			// ** The horizontal bloom blur pass **
			Framebuffer::Save();
			Framebuffer::Bind(*m_bloomHBlurOutputFramebuffer, 0, 0, m_bloomHBlurOutput->GetWidth(),
			                  m_bloomHBlurOutput->GetHeight());
			
			m_bloomHBlurShader.Use();
			glUniform2f(m_bloomHBlurUniformLocations.blurVector, 2.0f * m_pixelWidth, 0.0f);
			glUniform2f(m_bloomHBlurUniformLocations.texCoordOffset, 0.5f * m_pixelWidth, 0.5f * m_pixelHeight);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			
			
			// ** The vertical bloom blur pass **
			Framebuffer::Bind(*m_bloomVBlurOutputFramebuffer, 0, 0, m_bloomVBlurOutput->GetWidth(),
			                  m_bloomVBlurOutput->GetHeight());
			
			m_bloomHBlurOutput->Bind(1);
			m_bloomVBlurShader.Use();
			glUniform2f(m_bloomVBlurUniformLocations.blurVector, 0.0f, 2.0f * m_pixelHeight);
			glUniform2f(m_bloomVBlurUniformLocations.texCoordOffset, -0.5f * m_pixelWidth, -0.5f * m_pixelHeight);
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
		
		m_postProcessShader.Use();
		m_hexagonTexture.Bind(1);
		m_noiseTexture.Bind(3);
		
		glUniform1f(m_exposureUniformLoc,                   m_exposure);
		glUniform1f(m_gammaUniformLoc,                      m_gamma);
		glUniform1f(m_contrastUniformLoc,                   m_contrast * (1.0f - damageFlashIntensity * 0.7f));
		glUniform1f(m_framebufferARUniformLoc,              m_framebufferAR);
		glUniform1f(m_damageFlashIntensityUniformLoc,       damageFlashIntensity);
		glUniform1f(m_horizontalDistortionAmountUniformLoc, damageFlashIntensity * m_pixelWidth * 6);
		glUniform1f(m_distortionSampleMulUniformLoc,        m_distortionSampleMul);
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		if (m_blurAmount > 1E-6)
		{
			Framebuffer::Restore();
			m_blurPostProcess.DoBlurPass(m_blurAmount);
		}
	}
	
	void PostProcessor::DoDamageFlash()
	{
		m_damageFlashBeginTime = frameBeginTime;
	}
	
	void PostProcessor::SetBlurAmount(float blurAmount)
	{
		m_blurAmount = glm::clamp(blurAmount, 0.0f, 1.0f);
	}
	
	void PostProcessor::SetGamma(float gamma)
	{
		m_gamma = gamma;
	}
	
	void PostProcessor::OnResize(GLsizei newWidth, GLsizei newHeight)
	{
		m_blurInputFramebuffer = std::make_unique<Framebuffer>();
		m_blurInputBuffer = std::make_unique<Texture2D>(newWidth, newHeight, 1, GL_RGBA8);
		glNamedFramebufferTexture(m_blurInputFramebuffer->GetID(), GL_COLOR_ATTACHMENT0, m_blurInputBuffer->GetID(), 0);
		glNamedFramebufferDrawBuffer(m_blurInputFramebuffer->GetID(), GL_COLOR_ATTACHMENT0);
		m_blurInputBuffer->SetWrapMode(GL_CLAMP_TO_EDGE);
		m_blurInputBuffer->SetMinFilter(GL_LINEAR);
		m_blurInputBuffer->SetMagFilter(GL_LINEAR);
		
		m_blurPostProcess.CreateFramebuffer(*m_blurInputBuffer);
		
		if (!Settings::instance.EnableBloom())
		{
			m_bloomHBlurOutput = nullptr;
			m_bloomHBlurOutputFramebuffer = nullptr;
			
			m_bloomVBlurOutput = nullptr;
			m_bloomVBlurOutputFramebuffer = nullptr;
			
			return;
		}
		
		m_distortionSampleMul = 4.0f * static_cast<float>(newHeight) / static_cast<float>(NOISE_TEXTURE_RES);
		m_framebufferAR = static_cast<float>(newWidth) / static_cast<float>(newHeight);
		m_pixelWidth = 1.0f / newWidth;
		m_pixelHeight = 1.0f / newHeight;
		
		if (m_bloomHBlurOutput == nullptr || newWidth != m_bloomHBlurOutput->GetWidth() ||
			newHeight != m_bloomHBlurOutput->GetHeight())
		{
			m_bloomHBlurOutput = std::make_unique<Texture2D>(newWidth, newHeight, 1, GL_RGB16F);
			m_bloomHBlurOutput->SetWrapMode(GL_CLAMP_TO_EDGE);
			m_bloomHBlurOutput->SetMagFilter(GL_LINEAR);
			m_bloomHBlurOutput->SetMinFilter(GL_LINEAR);
			
			m_bloomHBlurOutputFramebuffer = std::make_unique<Framebuffer>();
			glNamedFramebufferTexture(m_bloomHBlurOutputFramebuffer->GetID(), GL_COLOR_ATTACHMENT0,
			                          m_bloomHBlurOutput->GetID(), 0);
			glNamedFramebufferDrawBuffer(m_bloomHBlurOutputFramebuffer->GetID(), GL_COLOR_ATTACHMENT0);
		}
		
		if (m_bloomVBlurOutput == nullptr || newWidth != m_bloomVBlurOutput->GetWidth() ||
			newHeight != m_bloomVBlurOutput->GetHeight())
		{
			m_bloomVBlurOutput = std::make_unique<Texture2D>(newWidth, newHeight, 1, GL_RGB16F);
			m_bloomVBlurOutput->SetWrapMode(GL_CLAMP_TO_EDGE);
			m_bloomVBlurOutput->SetMagFilter(GL_LINEAR);
			m_bloomVBlurOutput->SetMinFilter(GL_LINEAR);
			
			m_bloomVBlurOutputFramebuffer = std::make_unique<Framebuffer>();
			glNamedFramebufferTexture(m_bloomVBlurOutputFramebuffer->GetID(), GL_COLOR_ATTACHMENT0,
			                          m_bloomVBlurOutput->GetID(), 0);
			glNamedFramebufferDrawBuffer(m_bloomVBlurOutputFramebuffer->GetID(), GL_COLOR_ATTACHMENT0);
		}
	}
}
