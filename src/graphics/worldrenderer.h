#pragma once

#include <glm/glm.hpp>
#include "rendersettingsbuffer.h"
#include "spriterenderlist.h"
#include "irenderer.h"
#include "shadowrenderer.h"
#include "../world/entity.h"
#include "../world/path/path.h"

namespace TankGame
{
	class WorldRenderer : public IRenderer, private ShadowRenderer::IShadowMapGeometryProvider
	{
	public:
		inline void SetWorld(const class GameWorld* gameWorld)
		{ m_gameWorld = gameWorld; }
		
		void DrawShadowMaps(class ShadowRenderer& shadowRenderer, const class ViewInfo& viewInfo) const;
		
		void Prepare(const class ViewInfo& viewInfo, float gameTime) const;
		
		virtual void DrawGeometry(const class ViewInfo& viewInfo) const override;
		virtual void DrawTranslucentGeometry(const class ViewInfo& viewInfo) const override;
		
		virtual void DrawDistortions(const class ViewInfo& viewInfo) const override;
		virtual void DrawLighting(const class ViewInfo& viewInfo) const override;
		virtual void DrawParticles(const class ViewInfo& viewInfo, class ParticleRenderer& renderer) const override;
		
		inline size_t GetRenderedLights() const
		{ return m_lightSources.size(); }
		
		void UpdateResolution(int width, int height) const;
		
	private:
		virtual void DrawShadowCasters(const LightInfo& lightInfo, const class ViewInfo& viewInfo) const override;
		
		const class GameWorld* m_gameWorld = nullptr;
		
		mutable glm::ivec2 m_resolution;
		mutable RenderSettingsBuffer m_renderSettings;
		
		mutable SpriteRenderList m_spriteRenderList;
		mutable SpriteRenderList m_translucentSpriteRenderList;
		
		mutable std::vector<const class ILightSource*> m_lightSources;
		mutable std::vector<const Entity::IDistortionDrawable*> m_distortionDrawables;
	};
}
