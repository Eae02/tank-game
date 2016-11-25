#include "playmenu.h"
#include "../../updateinfo.h"
#include "../../graphics/ui/uirenderer.h"

namespace TankGame
{
	PlayMenu::PlayMenu() : m_backButton(U"Back")
	{
		
	}
	
	void PlayMenu::OnResize(int newWidth, int newHeight)
	{
		m_levelsList.SetDrawRectangle(Rectangle::CreateCentered(newWidth / 2.0f, newHeight / 2.0f,
		                                                        newWidth * 0.7f, newHeight * 0.8f));
		
		m_backButton.SetPosition(glm::vec2(newWidth / 2.0f, 100));
	}
	
	void PlayMenu::Update(const UpdateInfo& updateInfo)
	{
		if (m_backButton.Update(updateInfo) && m_backCallback != nullptr)
			m_backCallback();
		
		m_levelsList.Update(updateInfo);
	}
	
	void PlayMenu::Draw(const UIRenderer& uiRenderer) const
	{
		m_backButton.Draw(uiRenderer);
		m_levelsList.Draw(uiRenderer);
	}
	
	void PlayMenu::OnOpen()
	{
		m_backButton.OnShown();
	}
}
