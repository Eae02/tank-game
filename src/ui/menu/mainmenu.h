#pragma once

#include "../elements/menubutton.h"
#include "../../graphics/ui/uirenderer.h"
#include "../../graphics/gl/texture2d.h"

#include <functional>

namespace TankGame
{
	class MainMenu
	{
	public:
		MainMenu();
		
		void OnResize(int newWidth, int newHeight);
		
		void Update(const class UpdateInfo& updateInfo);
		void Draw(const UIRenderer& uiRenderer) const;
		
		void OnOpen();
		
		inline void SetPlayCallback(std::function<void()> playCallback)
		{ m_playCallback = std::move(playCallback); }
		inline void SetOptionsCallback(std::function<void()> optionsCallback)
		{ m_optionsCallback = std::move(optionsCallback); }
		inline void SetQuitCallback(std::function<void()> quitCallback)
		{ m_quitCallback = std::move(quitCallback); }
		
	private:
		void LayoutEntries();
		
		std::function<void()> m_playCallback;
		std::function<void()> m_optionsCallback;
		std::function<void()> m_quitCallback;
		
		Texture2D m_title;
		Rectangle m_titleRectangle;
		
		MenuButton m_playButton;
		MenuButton m_optionsButton;
		MenuButton m_quitButton;
		
		int m_screenWidth;
		int m_screenHeight;
	};
}
