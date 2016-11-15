#include "gamemanager.h"
#include "updateinfo.h"
#include "utils/ioutils.h"
#include "graphics/imainrenderer.h"

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
			m_level->Update(updateInfo);
		m_ambiencePlayer.Update();
	}
	
	void GameManager::DrawUI()
	{
		if (!m_level.IsNull())
			m_hudManager.DrawHUD();
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
