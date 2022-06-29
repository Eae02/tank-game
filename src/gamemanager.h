#pragma once

#include "level.h"
#include "audio/ambienceplayer.h"
#include "ui/hudmanager.h"
#include "graphics/worldrenderer.h"

namespace TankGame
{
	class GameManager
	{
	public:
		explicit GameManager(class IMainRenderer& renderer);
		
		void OnResize(GLsizei width, GLsizei height);
		
		void Update(const class UpdateInfo& updateInfo);
		
		void DrawUI();
		
		void SetLevel(Level&& level, bool testing = false);
		
		void ExitLevel();
		
		void Pause();
		
		inline bool IsPaused() const
		{ return m_hudManager.IsPaused(); }
		
		inline bool IsTesting() const
		{ return m_isTesting; }
		
		inline Level* GetLevel()
		{ return m_level.get(); }
		inline const Level* GetLevel() const
		{ return m_level.get(); }
		
		inline const WorldRenderer& GetRenderer() const
		{ return m_renderer; }
		inline WorldRenderer& GetRenderer()
		{ return m_renderer; }
		
		inline void LevelComplete(std::string nextLevelName)
		{ m_hudManager.ShowLevelComplete(nextLevelName); }
		
		inline void ShowNoAmmoText()
		{ m_hudManager.ShowNoAmmoText(); }
		
		inline void SetQuitCallback(std::function<void()> quitCallback)
		{ m_quitCallback = std::move(quitCallback); }
		
		inline void SetShowGlobalHealthBar(bool showGlobalHealthBar)
		{ m_hudManager.SetShowGlobalHealthBar(showGlobalHealthBar); }
		inline void SetGlobalHealthBarPercentage(float percentage)
		{ m_hudManager.SetGlobalHealthBarPercentage(percentage); }
		
	private:
		class IMainRenderer& m_mainRenderer;
		
		std::function<void()> m_quitCallback;
		
		float m_interactButtonOpacity = 0;
		glm::vec2 m_interactButtonPos;
		
		bool m_isTesting = false;
		
		std::unique_ptr<Level> m_level;
		
		HUDManager m_hudManager;
		
		AmbiencePlayer m_ambiencePlayer;
		
		WorldRenderer m_renderer;
	};
}
