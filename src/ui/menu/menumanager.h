#pragma once

#include "mainmenu.h"
#include "optionsmenu.h"
#include "playmenu.h"
#include "../../world/gameworld.h"
#include "../../utils/filesystem.h"
#include "../../graphics/worldrenderer.h"

#include <memory>
#include <nlohmann/json.hpp>

namespace TankGame
{
	enum class MenuScreens
	{
		MainMenu,
		Options,
		Play
	};
	
	class MenuManager
	{
	public:
		MenuManager();
		
		void OnResize(int newWidth, int newHeight);
		
		void ShowMainMenu();
		void Hide();
		
		bool Visible()
		{ return m_visible; }
		
		void Update(const class UpdateInfo& updateInfo);
		void Draw(class DeferredRenderer& deferredRenderer, const ShadowRenderer& shadowRenderer, float gameTime) const;
		
		inline void SetSettingsApplyCallback(OptionsMenu::ApplyCallback applyCallback)
		{ m_optionsMenu.m_applyCallback = std::move(applyCallback); }
		
		inline void SetQuitCallback(std::function<void()> quitCallback)
		{ m_mainMenu.SetQuitCallback(std::move(quitCallback)); }
		
		inline void SetLoadLevelCallback(LevelsList::LoadLevelCallback loadLevelCallback)
		{ m_playMenu.SetLoadLevelCallback(loadLevelCallback); }
		
	private:
		long GetBackgroundIndex(const std::string& levelName) const;
		
		void SetBackgroundWorld(std::unique_ptr<GameWorld>&& backgroundWorld);
		
		void SetCurrentMenu(MenuScreens menu);
		
		void UpdateViewInfo();
		
		struct Background
		{
			std::string m_levelName;
			glm::vec2 m_focusPosition;
			int m_requiredProgress;
			
			explicit Background(const nlohmann::json& element);
		};
		
		int m_screenWidth = 1;
		int m_screenHeight = 1;
		
		ViewInfo m_viewInfo;
		
		bool m_visible = false;
		
		float m_rotation = 0;
		
		std::vector<Background> m_backgrounds;
		long m_currentBackgroundIndex = -1;
		
		WorldRenderer m_worldRenderer;
		
		std::unique_ptr<GameWorld> m_backgroundWorld;
		
		MenuScreens m_currentMenu = MenuScreens::MainMenu;
		
		MainMenu m_mainMenu;
		OptionsMenu m_optionsMenu;
		PlayMenu m_playMenu;
	};
}
