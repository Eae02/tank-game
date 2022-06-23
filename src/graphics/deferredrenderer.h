#pragma once

#include "gl/framebuffer.h"
#include "gl/texture2d.h"
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
		
		const ParticleRenderer& GetParticleRenderer() const
		{ return m_particleRenderer; }
		
	private:
		static constexpr TextureFormat COLOR_FORMAT = TextureFormat::RGBA8;
		static constexpr TextureFormat NORMALS_AND_SPECULAR_FORMAT = TextureFormat::RGBA8;
		
		static constexpr TextureFormat DISTORTION_BUFFER_FORMAT = TextureFormat::RG16F;
		
		static constexpr TextureFormat LIGHT_ACC_FORMAT = TextureFormat::RGBA16F;
		
		std::unique_ptr<Framebuffer> m_geometryFramebuffer;
		std::unique_ptr<Texture2D> m_depthBuffer;
		std::unique_ptr<Texture2D> m_colorBuffer;
		std::unique_ptr<Texture2D> m_normalsAndSpecBuffer;
		
		std::unique_ptr<Framebuffer> m_distortionFramebuffer;
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
