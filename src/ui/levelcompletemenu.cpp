#include "levelcompletemenu.h"
#include "../graphics/ui/uirenderer.h"
#include "../graphics/ui/font.h"
#include "../keyboard.h"
#include "../mouse.h"
#include "../updateinfo.h"
#include "../utils/ioutils.h"

namespace TankGame
{
	LevelCompleteMenu::LevelCompleteMenu()
	    : m_title(Texture2D::FromFile(resDirectoryPath / "ui" / "level-complete.png")),
	      m_nextLevelButton(U"Next Level"), m_quitButton(U"Quit")
	{
		
	}
	
	void LevelCompleteMenu::Update(const UpdateInfo& updateInfo)
	{
		if (!m_nextLevelName.empty())
		{
			if (m_nextLevelButton.Update(updateInfo))
			{
				
			}
		}
		
		if (m_quitButton.Update(updateInfo))
			QuitClicked();
	}
	
	void LevelCompleteMenu::Draw(UIRenderer& uiRenderer) const
	{
		m_nextLevelButton.Draw(uiRenderer);
		m_quitButton.Draw(uiRenderer);
		
		uiRenderer.DrawSprite(m_title, m_titleRectangle, glm::vec4(1.0f));
	}
	
	void LevelCompleteMenu::OnResize(int newWidth, int newHeight)
	{
		m_screenWidth = newWidth;
		m_screenHeight = newHeight;
		LayoutEntries();
	}
	
	void LevelCompleteMenu::LayoutEntries()
	{
		const float ITEM_SPACING = 10;
		
		MenuButton* buttons[] = { &m_nextLevelButton, &m_quitButton };
		
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
	
	void LevelCompleteMenu::QuitClicked()
	{
		if (m_quitCallback != nullptr)
			m_quitCallback();
		m_shown = false;
	}
}
