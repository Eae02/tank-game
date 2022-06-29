#pragma once

#include "../graphics/gl/texture2d.h"
#include "../rectangle.h"

namespace TankGame
{
	class WeaponIcon
	{
	public:
		explicit WeaponIcon(int weaponIndex);
		
		void Update(const class UpdateInfo& updateInfo);
		void Draw(const class UIRenderer& uiRenderer) const;
		
		void SetRectangle(const Rectangle& rectangle);
		
		std::string GetInfoString() const;
		
		void SetWeaponState(const class PlayerWeaponState* weaponState);
		
		static void MaybeLoadIcons();
		
	private:
		static std::vector<Texture2D> s_icons;
		
		const class PlayerWeaponState* m_weaponState = nullptr;
		
		std::string m_incrementString;
		double m_incrementStringHideTime = 0;
		
		int m_lastAmmoCount = 0;
		
		float m_highlight = 0.0f;
		
		Rectangle m_rectangle;
		
		int m_weaponIndex;
	};
}
