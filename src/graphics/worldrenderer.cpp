#include "worldrenderer.h"
#include "quadmesh.h"
#include "viewinfo.h"
#include "tilegridmaterial.h"
#include "gl/shadermodule.h"
#include "../world/lights/ilightsource.h"
#include "../world/lights/ishadowlightsource.h"
#include "../world/gameworld.h"
#include "../utils/ioutils.h"
#include "../utils/utils.h"
#include "../profiling.h"

namespace TankGame
{
	void WorldRenderer::Prepare(const class ViewInfo& viewInfo, float gameTime) const
	{
		FUNC_TIMER
		if (m_gameWorld == nullptr)
			Panic("Invalid state: game world not set.");
		
		glm::vec2 centerPos = viewInfo.GetViewRectangle().Center();
		m_renderSettings.Update(viewInfo, glm::vec3(centerPos.x, 5, centerPos.y), gameTime);
		m_renderSettings.Bind();
	}
	
	void WorldRenderer::DrawGeometry(const ViewInfo& viewInfo) const
	{
		FUNC_TIMER
		if (m_gameWorld == nullptr)
			Panic("Invalid state: game world not set.");
		
		m_spriteRenderList.Begin();
		
		m_lightSources.clear();
		m_distortionDrawables.clear();
		
		m_gameWorld->IterateIntersectingEntities(viewInfo.GetViewRectangle(), [&] (const Entity& entity)
		{
			if (const ILightSource* lightSource = entity.AsLightSource())
			{
				LightInfo lightInfo = lightSource->GetLightInfo();
				if (viewInfo.Visible({ lightInfo.m_position, lightInfo.m_range }) && lightInfo.m_range > 1E-6)
				{
					m_lightSources.push_back(lightSource);
				}
			}
			
			if (const Entity::IDistortionDrawable* distortionDrawable = entity.AsDistortionDrawable())
			{
				if (viewInfo.Visible(entity.GetBoundingCircle()))
					m_distortionDrawables.push_back(distortionDrawable);
			}
			
			if (const Entity::ISpriteDrawable* spriteDrawable = entity.AsSpriteDrawable())
			{
				if (viewInfo.Visible(entity.GetBoundingCircle()))
					spriteDrawable->Draw(m_spriteRenderList);
			}
		});
		
		m_spriteRenderList.End();
		
		const TileGrid* tileGrid = m_gameWorld->GetTileGrid();
		const TileGridMaterial* tileGridMaterial = m_gameWorld->GetTileGridMaterial();
		
		if (tileGrid != nullptr && tileGridMaterial != nullptr)
			tileGrid->Draw(viewInfo, *tileGridMaterial);
	}
	
	void WorldRenderer::DrawTranslucentGeometry(const ViewInfo& viewInfo) const
	{
		FUNC_TIMER
		m_translucentSpriteRenderList.Begin();
		
		m_gameWorld->IterateIntersectingEntities(viewInfo.GetViewRectangle(), [&] (const Entity& entity)
		{
			if (const Entity::ITranslucentSpriteDrawable* translucentDrawable = entity.AsTranslucentSpriteDrawable())
			{
				if (viewInfo.Visible(entity.GetBoundingCircle()))
					translucentDrawable->DrawTranslucent(m_translucentSpriteRenderList);
			}
		});
		
		m_translucentSpriteRenderList.End(true);
	}
	
	void WorldRenderer::DrawDistortions(const ViewInfo& viewInfo) const
	{
		if (m_gameWorld == nullptr)
			Panic("Invalid state: game world not set.");
		
		for (const Entity::IDistortionDrawable* distortionDrawable : m_distortionDrawables)
		{
			distortionDrawable->DrawDistortions();
		}
	}
	
	void WorldRenderer::DrawLighting(const ViewInfo& viewInfo) const
	{
		FUNC_TIMER
		if (m_gameWorld == nullptr)
			Panic("Invalid state: game world not set.");
		
		QuadMesh::GetInstance().BindVAO();
		
		for (size_t i = 0; i < m_lightSources.size(); i++)
		{
			const ShaderProgram* shader = &m_lightSources[i]->GetShader();
			
			bool alreadyDrawn = false;
			for (size_t j = 0; j < i; j++)
			{
				if (&m_lightSources[j]->GetShader() == shader)
				{
					alreadyDrawn = true;
					break;
				}
			}
			if (alreadyDrawn)
				continue;
			
			shader->Use();
			
			for (size_t j = i; j < m_lightSources.size(); j++)
			{
				if (&m_lightSources[j]->GetShader() == shader)
				{
					m_lightSources[j]->Bind();
					
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				}
			}
		}
	}
	
	void WorldRenderer::DrawParticles(const ViewInfo& viewInfo, class ParticleRenderer& renderer) const
	{
		if (m_gameWorld == nullptr)
			Panic("Invalid state: game world not set.");
		
		m_gameWorld->IterateParticleEmitters([&] (const ParticleEmitter& emitter)
		{
			emitter.Render(renderer, viewInfo);
		});
	}
	
	void WorldRenderer::UpdateResolution(int width, int height) const
	{
		if (m_resolution.x == width && m_resolution.y == height)
			return;
		m_resolution = { width, height };
		m_renderSettings.OnResize(width, height);
	}
	
	void WorldRenderer::DrawShadowCasters(const LightInfo& lightInfo, const class ViewInfo& viewInfo) const
	{
		if (m_gameWorld == nullptr)
			Panic("Invalid state: game world not set.");
		
		const TileShadowCastersBuffer* shadowCastersBuffer = m_gameWorld->GetTileShadowCastersBuffer();
		
		if (shadowCastersBuffer != nullptr)
		{
			shadowCastersBuffer->Draw(lightInfo);
		}
	}
	
	void WorldRenderer::DrawShadowMaps(class ShadowRenderer& shadowRenderer, const ViewInfo& viewInfo) const
	{
		FUNC_TIMER
		if (m_gameWorld == nullptr)
			Panic("Invalid state: game world not set.");
		
		shadowRenderer.lastFrameShadowMapUpdates = 0;
		
		m_gameWorld->IterateIntersectingEntities(viewInfo.GetViewRectangle(), [&] (const Entity& entity)
		{
			const ILightSource* lightSource = entity.AsLightSource();
			if (lightSource == nullptr)
				return;
			
			if (auto shadowLightSource = dynamic_cast<const IShadowLightSource*>(lightSource))
			{
				class ShadowMap* shadowMap = shadowLightSource->GetShadowMap();
				
				if (shadowMap != nullptr)
					shadowRenderer.RenderShadowMap(*shadowMap, shadowLightSource->GetLightInfo(), viewInfo, *this);
			}
		});
	}
}
