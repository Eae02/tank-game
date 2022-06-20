#include "mainmenu.h"
#include "../../updateinfo.h"
#include "../../utils/ioutils.h"

namespace TankGame
{
	MainMenu::MainMenu()
	    : m_title(Texture2D::FromFile(resDirectoryPath / "ui" / "title.png")),
	      m_playButton(U"Play"), m_optionsButton(U"Options"), m_quitButton(U"Quit")
	{
		
	}
	
	void MainMenu::Update(const UpdateInfo& updateInfo)
	{
		if (m_playButton.Update(updateInfo) && m_playCallback != nullptr)
			m_playCallback();
		
		if (m_optionsButton.Update(updateInfo) && m_optionsCallback != nullptr)
			m_optionsCallback();
		
		if (m_quitButton.Update(updateInfo) && m_quitCallback != nullptr)
			m_quitCallback();
	}
	
	void MainMenu::Draw(const UIRenderer& uiRenderer) const
	{
		m_playButton.Draw(uiRenderer);
		m_optionsButton.Draw(uiRenderer);
		m_quitButton.Draw(uiRenderer);
		
		uiRenderer.DrawSprite(m_title, m_titleRectangle, glm::vec4(1.0f));
	}
	
	void MainMenu::OnOpen()
	{
		m_playButton.OnShown();
		m_optionsButton.OnShown();
		m_quitButton.OnShown();
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
		
		MenuButton* buttons[] = { &m_playButton, &m_optionsButton, &m_quitButton };
		
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
