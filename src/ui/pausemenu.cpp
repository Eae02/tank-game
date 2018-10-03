#include "pausemenu.h"
#include "../graphics/ui/uirenderer.h"
#include "../graphics/ui/font.h"
#include "../keyboard.h"
#include "../mouse.h"
#include "../updateinfo.h"
#include "../utils/ioutils.h"

namespace TankGame
{
	PauseMenu::PauseMenu()
	    : m_title(Texture2D::FromFile(GetResDirectory() / "ui" / "paused.png")),
	      m_resumeButton(U"Resume"), m_optionsButton(U"Options"), m_quitButton(U"Quit to Menu")
	{
		
	}
	
	void PauseMenu::Update(const UpdateInfo& updateInfo)
	{
		if (updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_ESCAPE) && !updateInfo.m_keyboard.WasKeyDown(GLFW_KEY_ESCAPE))
		{
			m_shown = !m_shown;
			
			if (m_shown)
			{
				MenuButton* buttons[] = { &m_resumeButton, &m_optionsButton, &m_quitButton };
				for (MenuButton* button : buttons)
					button->OnShown();
			}
		}
		
		if (!m_shown)
			return;
		
		if (m_resumeButton.Update(updateInfo))
			ResumeClicked();
		m_optionsButton.Update(updateInfo);
		if (m_quitButton.Update(updateInfo))
			QuitClicked();
	}
	
	void PauseMenu::Draw(UIRenderer& uiRenderer) const
	{
		if (!m_shown)
			return;
		
		m_resumeButton.Draw(uiRenderer);
		m_optionsButton.Draw(uiRenderer);
		m_quitButton.Draw(uiRenderer);
		
		uiRenderer.DrawSprite(m_title, m_titleRectangle, glm::vec4(1.0f));
	}
	
	void PauseMenu::OnResize(int newWidth, int newHeight)
	{
		m_screenWidth = newWidth;
		m_screenHeight = newHeight;
		LayoutEntries();
	}
	
	void PauseMenu::LayoutEntries()
	{
		const float ITEM_SPACING = 10;
		
		MenuButton* buttons[] = { &m_resumeButton, &m_optionsButton, &m_quitButton };
		
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
	
	void PauseMenu::SetShouldQuitToEditor(bool shouldQuitToEditor)
	{
		m_quitButton.SetLabel(shouldQuitToEditor ? U"Quit to Editor" : U"Quit to Menu");
		LayoutEntries();
	}
	
	void PauseMenu::ResumeClicked()
	{
		m_shown = false;
	}
	
	void PauseMenu::QuitClicked()
	{
		if (m_quitCallback != nullptr)
			m_quitCallback();
		m_shown = false;
	}
}
