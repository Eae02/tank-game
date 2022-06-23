#include "mainmenu.h"
#include "../../updateinfo.h"
#include "../../utils/ioutils.h"

namespace TankGame
{
	MainMenu::MainMenu()
	    : m_title(Texture2D::FromFile(resDirectoryPath / "ui" / "title.png", 4)),
	      m_playButton("Play"), m_optionsButton("Options") { }
	
	void MainMenu::Update(const UpdateInfo& updateInfo)
	{
		if (m_playButton.Update(updateInfo) && m_playCallback != nullptr)
			m_playCallback();
		
		if (m_optionsButton.Update(updateInfo) && m_optionsCallback != nullptr)
			m_optionsCallback();
		
#ifndef __EMSCRIPTEN__
		if (m_quitButton.Update(updateInfo) && m_quitCallback != nullptr)
			m_quitCallback();
#endif
	}
	
	void MainMenu::Draw(const UIRenderer& uiRenderer) const
	{
		m_playButton.Draw(uiRenderer);
		m_optionsButton.Draw(uiRenderer);
#ifndef __EMSCRIPTEN__
		m_quitButton.Draw(uiRenderer);
#endif
		
		uiRenderer.DrawSprite(m_title, m_titleRectangle, glm::vec4(1.0f));
	}
	
	void MainMenu::OnOpen()
	{
		m_playButton.OnShown();
		m_optionsButton.OnShown();
#ifndef __EMSCRIPTEN__
		m_quitButton.OnShown();
#endif
	}
	
	void MainMenu::OnResize(int newWidth, int newHeight)
	{
		m_screenWidth = newWidth;
		m_screenHeight = newHeight;
		LayoutEntries();
	}
	
	void MainMenu::LayoutEntries()
	{
		const float ITEM_SPACING = 10;
		
		MenuButton* buttons[] =
		{
			&m_playButton,
			&m_optionsButton,
#ifndef __EMSCRIPTEN__
			&m_quitButton
#endif
		};
		
		float height = -ITEM_SPACING;
		for (const MenuButton* button : buttons)
			height += button->GetHeight() + ITEM_SPACING;
		
		float y = (m_screenHeight + height) / 2.0f;
		
		m_titleRectangle.x = (m_screenWidth - m_title.GetWidth()) / 2.0f;
		m_titleRectangle.y = y + 20;
		m_titleRectangle.w = m_title.GetWidth();
		m_titleRectangle.h = m_title.GetHeight();
		
		for (MenuButton* button : buttons)
		{
			y -= button->GetHeight();
			button->SetPosition({ m_screenWidth / 2.0f, y + button->GetHeight() / 2.0f });
			y -= ITEM_SPACING;
		}
	}
}
