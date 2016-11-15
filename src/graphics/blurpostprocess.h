#pragma once

#include "gl/texture2d.h"
#include "gl/framebuffer.h"
#include "gl/shaderprogram.h"
#include "../utils/memory/stackobject.h"

namespace TankGame
{
	class BlurPostProcess
	{
	public:
		BlurPostProcess();
		
		void CreateFramebuffer(const Texture2D& inputTexture);
		
		void DoBlurPass(float blurAmount) const;
		
	private:
		const Texture2D* m_input = nullptr;
		
		ShaderProgram m_shader;
		
		int m_blurVectorUniformLocation;
		
		StackObject<Framebuffer> m_framebuffer;
		StackObject<Texture2D> m_intermidiateBuffer;
	};
}
