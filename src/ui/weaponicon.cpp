#include "weaponicon.h"
#include "hudmanager.h"
#include "../updateinfo.h"
#include "../graphics/ui/uirenderer.h"
#include "../graphics/ui/font.h"
#include "../utils/ioutils.h"
#include "../world/entities/playerweaponstate.h"

#include <sstream>
#include <GLFW/glfw3.h>
#include <iostream>

namespace TankGame
{
	std::vector<Texture2D> WeaponIcon::s_icons;
	
	WeaponIcon::WeaponIcon(int weaponIndex)
	{
		m_weaponIndex = weaponIndex;
	}
	
	void WeaponIcon::Update(const UpdateInfo& updateInfo)
	{
		if (m_weaponState == nullptr)
			return;
		
		bool isSelected = HUDManager::GetWeaponIndex(*m_weaponState) == m_weaponIndex;
		UpdateTransition(m_highlight, isSelected ? 1.0f : 0.0f, updateInfo.m_dt * 5);
		
		int ammoCount = m_weaponState->GetAmmoCount(static_cast<SpecialWeapons>(m_weaponIndex - 1));
		if (ammoCount > m_lastAmmoCount)
		{
			m_incrementString = UTF8ToUTF32("+" + std::to_string(ammoCount - m_lastAmmoCount));
			m_incrementStringHideTime = glfwGetTime() + 2.0;
		}
		
		m_lastAmmoCount = ammoCount;
	}
	
	void WeaponIcon::Draw(const UIRenderer& uiRenderer) const
	{
		glm::vec4 color(1, 1, 1, glm::mix(0.5f, 1.0f, m_highlight));
		
		uiRenderer.DrawSprite(s_icons[m_weaponIndex], m_rectangle, color);
		
		double incrementStringTimeLeft = m_incrementStringHideTime - glfwGetTime();
		if (incrementStringTimeLeft > 0 && !m_incrementString.empty())
		{
			const Font& font = Font::GetNamedFont(FontNames::ButtonFont);
			
			glm::vec4 color(ParseColorHexCodeSRGB(0xED7F09), glm::min<float>(incrementStringTimeLeft, 1.0f));
			Rectangle incStringRect(m_rectangle.x, m_rectangle.FarY(), m_rectangle.w, font.GetSize());
			
			uiRenderer.DrawString(font, m_incrementString, incStringRect, Alignment::Center,
			                      Alignment::Center, color, 0.75f);
		}
	}
	
	void WeaponIcon::SetRectangle(const Rectangle& rectangle)
	{
		m_rectangle = rectangle;
	}
	
	std::string WeaponIcon::GetInfoString() const
	{
		if (m_weaponState == nullptr)
			return "";
		
		if (m_weaponIndex == 0)
			return "Plasma Gun";
		
		SpecialWeapons weapon = static_cast<SpecialWeapons>(m_weaponIndex - 1);
		
		std::ostringstream infoStringStream;
		infoStringStream << GetWeaponDisplayName(weapon) << " [" << m_weaponState->GetAmmoCount(weapon) << "]";
		
		return infoStringStream.str();
	}
	
	void WeaponIcon::SetWeaponState(const PlayerWeaponState* weaponState)
	{
		m_weaponState = weaponState;
		
		if (m_weaponIndex != 0 && weaponState != nullptr)
		{
			m_incrementStringHideTime = glfwGetTime();
			m_lastAmmoCount = weaponState->GetAmmoCount(static_cast<SpecialWeapons>(m_weaponIndex - 1));
		}
	}
	
	void WeaponIcon::MaybeLoadIcons()
	{
		if (!s_icons.empty())
			return;
		
		s_icons.push_back(Texture2D::FromFile(GetResDirectory() / "ui" / "weapon-plasma-gun.png"));
		s_icons.push_back(Texture2D::FromFile(GetResDirectory() / "ui" / "weapon-rocket-launcher.png"));
	}
}
