#include "gamemanager.h"
#include "updateinfo.h"
#include "utils/ioutils.h"
#include "graphics/ui/drawbutton.h"
#include "world/entities/playerentity.h"
#include "graphics/imainrenderer.h"
#include "settings.h"
#include "profiling.h"

namespace TankGame
{
	GameManager::GameManager(IMainRenderer& renderer)
	    : m_mainRenderer(renderer), m_hudManager(renderer), m_ambiencePlayer(resDirectoryPath / "audio" / "ambient")
	{
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
		FUNC_TIMER
		if (m_level == nullptr)
			return;
		
		m_hudManager.Update(updateInfo);
		if (!m_hudManager.IsPaused() && m_level != nullptr)
		{
			float targetInteractButtonOpacity = 0.0f;
			m_level->GetGameWorld().IterateIntersectingEntities(m_level->GetPlayerEntity().GetInteractRectangle(),
			                                                    [&] (Entity& entity)
			{
				if (entity.CanInteract())
				{
					targetInteractButtonOpacity = 1.0f;
					m_interactButtonPos = updateInfo.m_viewInfo.WorldToScreen(entity.GetTransform().GetPosition());
				}
			});
			
			UpdateTransition(m_interactButtonOpacity, targetInteractButtonOpacity, updateInfo.m_dt * 5.0f);
			
			m_level->Update(updateInfo);
		}
		else
			m_interactButtonOpacity = 0.0f;
		
		m_ambiencePlayer.Update();
	}
	
	void GameManager::DrawUI()
	{
		if (m_level != nullptr)
		{
			m_hudManager.DrawHUD();
			
			if (m_interactButtonOpacity > (1 / 255.0f))
			{
				glm::vec2 pos = m_interactButtonPos * glm::vec2(UIRenderer::GetInstance().GetWindowWidth(),
				                                                UIRenderer::GetInstance().GetWindowHeight());
				
				DrawButton("E", UIRenderer::GetInstance(), pos, 40, m_interactButtonOpacity * 0.9f);
			}
		}
	}
	
	void GameManager::SetLevel(Level&& level, bool testing)
	{
		m_level = std::make_unique<Level>(std::move(level));
		
		m_level->GetGameWorld().SetGameManager(this);
		m_level->GetGameWorld().SetRenderer(&m_mainRenderer);
		
		m_hudManager.SetPlayerEntity(&m_level->GetPlayerEntity());
		m_renderer.SetWorld(&m_level->GetGameWorld());
		m_ambiencePlayer.Start();
		
		m_isTesting = testing;
		m_hudManager.SetShouldQuitToEditor(m_isTesting);
		
		m_hudManager.SetShowGlobalHealthBar(false);
		m_hudManager.SetGlobalHealthBarPercentage(1.0f);
	}
	
	void GameManager::ExitLevel()
	{
		m_mainRenderer.SetBlurAmount(0);
		m_hudManager.SetPlayerEntity(nullptr);
		
		m_level = nullptr;
		m_ambiencePlayer.Stop();
	}
	
	void GameManager::Pause()
	{
		m_hudManager.ShowPauseMenu();
	}
}
