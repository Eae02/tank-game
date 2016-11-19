#pragma once

#include "gl/bufferallocator.h"
#include "gl/texture2d.h"
#include "gl/framebuffer.h"
#include "gl/shaderprogram.h"
#include "noisetexture.h"
#include "blurpostprocess.h"
#include "../utils/memory/stackobject.h"
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
		
	private:
		static ShaderProgram LoadShader(const fs::path& path);
		
		float m_exposure = 1.5f;
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
		
		StackObject<Texture2D> m_bloomHBlurOutput;
		StackObject<Texture2D> m_bloomVBlurOutput;
		
		StackObject<Framebuffer> m_bloomHBlurOutputFramebuffer;
		StackObject<Framebuffer> m_bloomVBlurOutputFramebuffer;
		
		StackObject<Framebuffer> m_blurInputFramebuffer;
		StackObject<Texture2D> m_blurInputBuffer;
		BlurPostProcess m_blurPostProcess;
		
		ShaderProgram m_bloomHBlurShader;
		ShaderProgram m_bloomVBlurShader;
		ShaderProgram m_postProcessShader;
		
		BufferAllocator::UniquePtr m_blurVectorsBuffer;
		BufferAllocator::UniquePtr m_postSettingsUB;
		mutable bool m_postSettingsNeedUpload = true;
	};
}
