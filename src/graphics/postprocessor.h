#pragma once

#include "gl/bufferallocator.h"
#include "gl/texture2d.h"
#include "gl/framebuffer.h"
#include "gl/shaderprogram.h"
#include "noisetexture.h"
#include "blurpostprocess.h"
#include "../utils/filesystem.h"

namespace TankGame
{
	class PostProcessor
	{
	public:
		PostProcessor();
		
		void OnResize(GLsizei newWidth, GLsizei newHeight);
		
		void DoPostProcessing(const Texture2D& inputTexture, const Texture2D& distortionsTexture) const;
		
		void DoDamageFlash();
		
		void SetBlurAmount(float blurAmount);
		
		void SetGamma(float gamma);
		inline float GetGamma() const
		{ return m_gamma; }
		
	private:
		static ShaderProgram LoadShader(const fs::path& path);
		
		float m_exposure = 1.75f;
		float m_gamma = 1.0f;
		float m_contrast = 1.0f;
		float m_framebufferAR = 1.0f;
		
		double m_damageFlashBeginTime = 0;
		mutable float m_oldDamageFlashIntensity = 0;
		
		float m_blurAmount = 0;
		
		float m_distortionSampleMul = 0;
		float m_pixelWidth = 0;
		
		Texture2D m_hexagonTexture;
		NoiseTexture m_noiseTexture;
		
		std::unique_ptr<Texture2D> m_bloomHBlurOutput;
		std::unique_ptr<Texture2D> m_bloomVBlurOutput;
		
		std::unique_ptr<Framebuffer> m_bloomHBlurOutputFramebuffer;
		std::unique_ptr<Framebuffer> m_bloomVBlurOutputFramebuffer;
		
		std::unique_ptr<Framebuffer> m_blurInputFramebuffer;
		std::unique_ptr<Texture2D> m_blurInputBuffer;
		BlurPostProcess m_blurPostProcess;
		
		ShaderProgram m_bloomHBlurShader;
		ShaderProgram m_bloomVBlurShader;
		ShaderProgram m_postProcessShader;
		
		BufferAllocator::UniquePtr m_blurVectorsBuffer;
		BufferAllocator::UniquePtr m_postSettingsUB;
		mutable bool m_postSettingsNeedUpload = true;
	};
}
