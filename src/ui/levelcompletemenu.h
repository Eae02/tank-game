#pragma once

#include <functional>

#include "elements/menubutton.h"
#include "../rectangle.h"
#include "../graphics/gl/texture2d.h"

namespace TankGame
{
	class LevelCompleteMenu
	{
	public:
		LevelCompleteMenu();
		
		void Update(const class UpdateInfo& updateInfo);
		
		void Draw(class UIRenderer& uiRenderer) const;
		
		void OnResize(int newWidth, int newHeight);
		
		inline bool IsShown() const
		{ return m_shown; }
		
		inline void Show(std::string nextLevelName = "")
		{
			m_nextLevelName = std::move(nextLevelName);
			m_shown = true;
		}
		
		inline void SetQuitCallback(std::function<void()> callback)
		{ m_quitCallback = std::move(callback); }
		
	private:
		void ResumeClicked();
		void QuitClicked();
		void LayoutEntries();
		
		std::string m_nextLevelName;
		
		Texture2D m_title;
		Rectangle m_titleRectangle;
		
		std::function<void()> m_quitCallback;
		
		MenuButton m_nextLevelButton;
		MenuButton m_quitButton;
		
		int m_screenWidth;
		int m_screenHeight;
		
		bool m_shown = false;
	};
}
