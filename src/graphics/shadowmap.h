#pragma once

#include "viewinfo.h"
#include "gl/texture2d.h"
#include "gl/framebuffer.h"
#include "gl/bufferallocator.h"
#include "../world/lights/ilightsource.h"

#include <memory>

namespace TankGame
{
	class ShadowMap
	{
	public:
		explicit ShadowMap(bool isStatic);
		
		void SetResolution(GLsizei width, GLsizei height);
		inline GLsizei GetWidth() const { return m_width; }
		inline GLsizei GetHeight() const { return m_height; }
		
		inline void SetGameWorld(const class GameWorld& gameWorld)
		{ m_gameWorld = &gameWorld; }
		
		glm::mat3 GetInverseStaticViewMatrix(LightInfo lightInfo) const;
		glm::mat3 GetStaticViewMatrix(LightInfo lightInfo) const;
		
		void BeginShadowPass(const ViewInfo& viewInfo, LightInfo lightInfo) const;
		void BeginBlurPass() const;
		
		void Bind() const;
		static void BindDefault();
		
		inline bool IsStatic() const
		{ return m_isStatic; }
		
		inline bool NeedsRedraw() const
		{ return !m_isStatic || m_needsRedraw; }
		inline void ClearNeedsRedraw()
		{ m_needsRedraw = false; }
		inline void FlagForRedraw()
		{ m_needsRedraw = true; }
		
		inline float GetStaticQualityMultiplier() const
		{ return m_staticQualityMultiplier; }
		inline void SetStaticQualityMultiplier(float staticQualityMultiplier)
		{ m_staticQualityMultiplier = staticQualityMultiplier; }
		
	private:
		static std::unique_ptr<Texture2D> s_defaultTexture;
		static std::unique_ptr<Buffer> s_defaultRenderSettingsBuffer;
		
		const class GameWorld* m_gameWorld;
		
		std::unique_ptr<Texture2D> m_shadowPassTexture;
		std::unique_ptr<Framebuffer> m_shadowPassFramebuffer;
		
		std::unique_ptr<Texture2D> m_blurPassTexture;
		std::unique_ptr<Framebuffer> m_blurPassFramebuffer;
		
		BufferAllocator::UniquePtr m_renderSettingsBuffer;
		
		bool m_needsRedraw = true;
		bool m_isStatic;
		float m_staticQualityMultiplier = 1.0f;
		
		GLsizei m_width = 0;
		GLsizei m_height = 0;
	};
}
