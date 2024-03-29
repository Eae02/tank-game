#include "deferredrenderer.h"
#include "irenderer.h"
#include "quadmesh.h"
#include "gl/shadermodule.h"
#include "../utils/ioutils.h"
#include "../settings.h"
#include "../profiling.h"

namespace TankGame
{
	constexpr TextureFormat DeferredRenderer::COLOR_FORMAT;
	constexpr TextureFormat DeferredRenderer::NORMALS_AND_SPECULAR_FORMAT;
	constexpr TextureFormat DeferredRenderer::DISTORTION_BUFFER_FORMAT;
	constexpr TextureFormat DeferredRenderer::LIGHT_ACC_FORMAT;
	
	static ShaderProgram LoadCompositionShader()
	{
		ShaderProgram program(QuadMesh::GetVertexShader(), ShaderModule::FromResFile("lighting/composition.fs.glsl"));
		program.SetTextureBinding("colorSampler", 0);
		program.SetTextureBinding("lightAccSampler", 1);
		return program;
	}
	
	DeferredRenderer::DeferredRenderer()
	    : m_compositionShader(LoadCompositionShader()) {  }
	
	void DeferredRenderer::CreateFramebuffer(int width, int height)
	{
		m_resolutionScale = Settings::instance.GetResolutionScale();
		
		double resScale = static_cast<double>(Settings::instance.GetResolutionScale()) / 100.0;
		int scaledW = static_cast<double>(width) * resScale;
		int scaledH = static_cast<double>(height) * resScale;
		
		m_geometryFramebuffer = std::make_unique<Framebuffer>();
		
		m_depthBuffer = std::make_unique<Texture2D>(scaledW, scaledH, 1, TextureFormat::Depth16);
		m_depthBuffer->SetupMipmapping(false);
		
		m_colorBuffer = std::make_unique<Texture2D>(scaledW, scaledH, 1, COLOR_FORMAT);
		m_colorBuffer->SetupMipmapping(false);
		m_colorBuffer->SetWrapS(GL_CLAMP_TO_EDGE);
		m_colorBuffer->SetWrapT(GL_CLAMP_TO_EDGE);
		m_colorBuffer->SetMinFilter(GL_LINEAR);
		m_colorBuffer->SetMagFilter(GL_LINEAR);
		
		m_normalsAndSpecBuffer = std::make_unique<Texture2D>(scaledW, scaledH, 1, NORMALS_AND_SPECULAR_FORMAT);
		m_normalsAndSpecBuffer->SetupMipmapping(false);
		m_normalsAndSpecBuffer->SetWrapS(GL_CLAMP_TO_EDGE);
		m_normalsAndSpecBuffer->SetWrapT(GL_CLAMP_TO_EDGE);
		m_normalsAndSpecBuffer->SetMinFilter(GL_LINEAR);
		m_normalsAndSpecBuffer->SetMagFilter(GL_LINEAR);
		
		glNamedFramebufferTexture(m_geometryFramebuffer->GetID(), GL_COLOR_ATTACHMENT0, m_colorBuffer->GetID(), 0);
		glNamedFramebufferTexture(m_geometryFramebuffer->GetID(), GL_COLOR_ATTACHMENT1, m_normalsAndSpecBuffer->GetID(), 0);
		glNamedFramebufferTexture(m_geometryFramebuffer->GetID(), GL_DEPTH_ATTACHMENT, m_depthBuffer->GetID(), 0);
		
		
		m_distortionFramebuffer = std::make_unique<Framebuffer>();
		
		m_distortionBuffer = std::make_unique<Texture2D>(scaledW, scaledH, 1, DISTORTION_BUFFER_FORMAT);
		m_distortionBuffer->SetupMipmapping(false);
		m_distortionBuffer->SetWrapMode(GL_CLAMP_TO_EDGE);
		m_distortionBuffer->SetMinFilter(GL_LINEAR);
		m_distortionBuffer->SetMagFilter(GL_LINEAR);
		
		glNamedFramebufferTexture(m_distortionFramebuffer->GetID(), GL_COLOR_ATTACHMENT0, m_distortionBuffer->GetID(), 0);
		glNamedFramebufferDrawBuffer(m_distortionFramebuffer->GetID(), GL_COLOR_ATTACHMENT0);
		
		
		m_lightFramebuffer = std::make_unique<Framebuffer>();
		
		m_lightAccBuffer = std::make_unique<Texture2D>(scaledW, scaledH, 1, LIGHT_ACC_FORMAT);
		m_lightAccBuffer->SetupMipmapping(false);
		m_lightAccBuffer->SetWrapMode(GL_CLAMP_TO_EDGE);
		m_lightAccBuffer->SetMinFilter(GL_LINEAR);
		m_lightAccBuffer->SetMagFilter(GL_LINEAR);
		
		glNamedFramebufferTexture(m_lightFramebuffer->GetID(), GL_COLOR_ATTACHMENT0, m_lightAccBuffer->GetID(), 0);
		glNamedFramebufferDrawBuffer(m_lightFramebuffer->GetID(), GL_COLOR_ATTACHMENT0);
		
		m_outputFramebuffer = std::make_unique<Framebuffer>();
		
		m_outputBuffer = std::make_unique<Texture2D>(width, height, 1, LIGHT_ACC_FORMAT);
		m_outputBuffer->SetupMipmapping(false);
		m_outputBuffer->SetWrapMode(GL_CLAMP_TO_EDGE);
		
		glNamedFramebufferTexture(m_outputFramebuffer->GetID(), GL_COLOR_ATTACHMENT0, m_outputBuffer->GetID(), 0);
		glNamedFramebufferDrawBuffer(m_outputFramebuffer->GetID(), GL_COLOR_ATTACHMENT0);
		
		m_postProcessor.OnResize(width, height);
	}
	
	bool DeferredRenderer::FramebufferOutOfDate() const
	{
		return m_resolutionScale != Settings::instance.GetResolutionScale();
	}
	
	void DeferredRenderer::Draw(const IRenderer& renderer, const class ViewInfo& viewInfo) const
	{
		FUNC_TIMER
		
		Framebuffer::Save();
		Framebuffer::Bind(*m_geometryFramebuffer, 0, 0, m_colorBuffer->GetWidth(), m_colorBuffer->GetHeight());
		
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		const float depthClear = 1.0f;
		
		glEnable(GL_DEPTH_TEST);
		
		// ** Geometry pass **
		GLenum geometryDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glNamedFramebufferDrawBuffers(m_geometryFramebuffer->GetID(), 2, geometryDrawBuffers);
		glDepthMask(GL_TRUE);
		
		glClearBufferfv(GL_COLOR, 0, clearColor);
		glClearBufferfv(GL_COLOR, 1, clearColor);
		glClearBufferfv(GL_DEPTH, 0, &depthClear);
		
		renderer.DrawGeometry(viewInfo);
		
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		glDepthMask(GL_FALSE);
		
		glNamedFramebufferDrawBuffer(m_geometryFramebuffer->GetID(), GL_COLOR_ATTACHMENT0);
		
		renderer.DrawTranslucentGeometry(viewInfo);
		
		// ** Distortion pass **
		Framebuffer::Bind(*m_distortionFramebuffer, 0, 0, m_distortionBuffer->GetWidth(), m_distortionBuffer->GetHeight());
		
		//Enables additive blending for this pass and the light accumulation pass
		glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ZERO);
		
		glClearBufferfv(GL_COLOR, 0, clearColor);
		
		renderer.DrawDistortions(viewInfo);
		
		glDisable(GL_DEPTH_TEST);
		
		// ** Light pass **
		Framebuffer::Bind(*m_lightFramebuffer, 0, 0, m_lightAccBuffer->GetWidth(), m_lightAccBuffer->GetHeight());
		
		m_normalsAndSpecBuffer->Bind(0);
		
		glClearBufferfv(GL_COLOR, 0, clearColor);
		
		renderer.DrawLighting(viewInfo);
		
		glDisable(GL_BLEND);
		
		// ** Composition pass **
		Framebuffer::Bind(*m_outputFramebuffer, 0, 0, m_outputBuffer->GetWidth(), m_outputBuffer->GetHeight());
		
		m_colorBuffer->Bind(0);
		m_lightAccBuffer->Bind(1);
		
		m_compositionShader.Use();
		
		QuadMesh::GetInstance().BindVAO();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		m_particleRenderer.Begin(*m_lightAccBuffer);
		renderer.DrawParticles(viewInfo, m_particleRenderer);
		m_particleRenderer.End();
		
		Framebuffer::Restore();
		m_postProcessor.DoPostProcessing(*m_outputBuffer, *m_distortionBuffer);
	}
}
