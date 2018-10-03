#pragma once

#include "gl/framebuffer.h"
#include "gl/texture2d.h"
#include "gl/renderbuffer.h"
#include "gl/shaderprogram.h"
#include "imainrenderer.h"
#include "postprocessor.h"
#include "particlerenderer.h"

#include "../settings.h"

#include <memory>

namespace TankGame
{
	class DeferredRenderer : public IMainRenderer
	{
	public:
		DeferredRenderer();
		
		void CreateFramebuffer(int width, int height);
		
		bool FramebufferOutOfDate() const;
		
		void Draw(const class IRenderer& renderer, const class ViewInfo& viewInfo) const;
		
		inline const Texture2D& GetLightBuffer() const
		{ return *m_lightAccBuffer; }
		
		virtual void DoDamageFlash() final override
		{ m_postProcessor.DoDamageFlash(); }
		
		virtual void SetBlurAmount(float blurAmount) final override
		{ m_postProcessor.SetBlurAmount(blurAmount); }
		
		inline void SetGamma(float gamma)
		{ m_postProcessor.SetGamma(gamma); }
		inline float GetGamma() const
		{ return m_postProcessor.GetGamma(); }
		
	private:
		static constexpr GLenum COLOR_FORMAT = GL_RGBA8;
		static constexpr GLenum NORMALS_AND_SPECULAR_FORMAT = GL_RGBA8;
		
		static constexpr GLenum DISTORTION_BUFFER_FORMAT = GL_RG16F;
		
		static constexpr GLenum LIGHT_ACC_FORMAT = GL_RGB16F;
		
		std::unique_ptr<Framebuffer> m_geometryFramebuffer;
		std::unique_ptr<Renderbuffer> m_depthBuffer;
		std::unique_ptr<Texture2D> m_colorBuffer;
		std::unique_ptr<Texture2D> m_normalsAndSpecBuffer;
		std::unique_ptr<Texture2D> m_distortionBuffer;
		
		std::unique_ptr<Framebuffer> m_lightFramebuffer;
		std::unique_ptr<Texture2D> m_lightAccBuffer;
		
		std::unique_ptr<Framebuffer> m_outputFramebuffer;
		std::unique_ptr<Texture2D> m_outputBuffer;
		
		ResolutionScales m_resolutionScale = ResolutionScales::_100;
		
		PostProcessor m_postProcessor;
		
		mutable ParticleRenderer m_particleRenderer;
		
		ShaderProgram m_compositionShader;
	};
}
