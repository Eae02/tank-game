#pragma once

#include "../levelslist.h"
#include "../elements/menubutton.h"

#include <functional>

namespace TankGame
{
	class PlayMenu
	{
	public:
		PlayMenu();
		
		void OnResize(int newWidth, int newHeight);
		
		void Update(const class UpdateInfo& updateInfo);
		void Draw(const class UIRenderer& uiRenderer) const;
		
		inline void SetBackCallback(std::function<void()> backCallback)
		{ m_backCallback = std::move(backCallback); }
		inline void SetLoadLevelCallback(LevelsList::LoadLevelCallback loadLevelCallback)
		{ m_levelsList.SetLoadLevelCallback(loadLevelCallback); }
		
		void OnOpen();
		
	private:
		LevelsList m_levelsList;
		
		MenuButton m_backButton;
		
		std::function<void()> m_backCallback;
	};
}
