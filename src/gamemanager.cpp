#include "gamemanager.h"
#include "updateinfo.h"
#include "utils/ioutils.h"
#include "graphics/ui/drawbutton.h"
#include "world/entities/playerentity.h"
#include "graphics/imainrenderer.h"
#include "settings.h"

#include <GLFW/glfw3.h>

namespace TankGame
{
	GameManager::GameManager(IMainRenderer& renderer)
	    : m_mainRenderer(renderer), m_hudManager(renderer), m_ambiencePlayer(GetResDirectory() / "audio" / "ambient")
	{
		m_ambiencePlayer.SetVolume(0.5f);
		
		m_hudManager.SetQuitCallback([this] 
		{
			ExitLevel();
			if (m_quitCallback != nullptr)
				m_quitCallback();
		});
	}
	
	void GameManager::OnResize(GLsizei width, GLsizei height)
	{
		m_hudManager.OnResize(width, height);
	}
	
	void GameManager::Update(const UpdateInfo& updateInfo)
	{
		if (m_level.IsNull())
			return;
		
		m_hudManager.Update(updateInfo);
		if (!m_hudManager.IsPaused() && !m_level.IsNull())
		{
			float targetInteractButtonOpacity = 0.0f;
			m_level->GetGameWorld().IterateIntersectingEntities(m_level->GetPlayerEntity().GetInteractRectangle(),
																[&] (const Entity& entity)
			{
				if (entity.CanInteract())
				{
					targetInteractButtonOpacity = 1.0f;
					m_interactButtonPos = updateInfo.m_viewInfo.WorldToScreen(entity.GetTransform().GetPosition());
				}
			});
			
			float deltaInteractButtonOpacity = targetInteractButtonOpacity - m_interactButtonOpacity;
			m_interactButtonOpacity += glm::min(updateInfo.m_dt * 5.0f, glm::abs(deltaInteractButtonOpacity)) *
									   glm::sign(deltaInteractButtonOpacity);
			
			m_level->Update(updateInfo);
		}
		else
			m_interactButtonOpacity = 0.0f;
		
		m_ambiencePlayer.Update();
	}
	
	void GameManager::DrawUI()
	{
		if (!m_level.IsNull())
		{
			m_hudManager.DrawHUD();
			
			if (m_interactButtonOpacity > (1 / 255.0f))
			{
				glm::vec2 pos = m_interactButtonPos * glm::vec2(UIRenderer::GetInstance().GetWindowWidth(),
				                                                UIRenderer::GetInstance().GetWindowHeight());
				
				DrawButton(Settings::GetInstance().GetInteractButton(), UIRenderer::GetInstance(), pos, 40,
				           m_interactButtonOpacity * 0.9f);
			}
		}
	}
	
	void GameManager::SetLevel(Level&& level, bool testing)
	{
		m_level.Construct(std::move(level));
		
		m_level->GetGameWorld().SetRenderer(&m_mainRenderer);
		
		m_hudManager.SetPlayerEntity(&m_level->GetPlayerEntity());
		m_renderer.SetWorld(&m_level->GetGameWorld());
		m_ambiencePlayer.Start();
		
		m_isTesting = testing;
		m_hudManager.SetShouldQuitToEditor(m_isTesting);
	}
	
	void GameManager::ExitLevel()
	{
		m_mainRenderer.SetBlurAmount(0);
		
		m_level.Destroy();
		m_ambiencePlayer.Stop();
	}
	
	void GameManager::Pause()
	{
		m_hudManager.ShowPauseMenu();
	}
}
