#pragma once

#include "gl/framebuffer.h"
#include "gl/texture2d.h"
#include "gl/renderbuffer.h"
#include "gl/shaderprogram.h"
#include "imainrenderer.h"
#include "postprocessor.h"
#include "particlerenderer.h"

#include "../utils/memory/stackobject.h"
#include "../settings.h"

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
		static constexpr GLenum COLOR_FORMAT = GL_RGBA16F;
		static constexpr GLenum NORMALS_AND_SPECULAR_FORMAT = GL_RGBA16F;
		
		static constexpr GLenum DISTORTION_BUFFER_FORMAT = GL_RG16F;
		
		static constexpr GLenum LIGHT_ACC_FORMAT = GL_RGB16F;
		
		StackObject<Framebuffer> m_geometryFramebuffer;
		StackObject<Renderbuffer> m_depthBuffer;
		StackObject<Texture2D> m_colorBuffer;
		StackObject<Texture2D> m_normalsAndSpecBuffer;
		StackObject<Texture2D> m_distortionBuffer;
		
		StackObject<Framebuffer> m_lightFramebuffer;
		StackObject<Texture2D> m_lightAccBuffer;
		
		StackObject<Framebuffer> m_outputFramebuffer;
		StackObject<Texture2D> m_outputBuffer;
		
		ResolutionScales m_resolutionScale = ResolutionScales::_100;
		
		PostProcessor m_postProcessor;
		
		mutable ParticleRenderer m_particleRenderer;
		
		ShaderProgram m_compositionShader;
	};
}
