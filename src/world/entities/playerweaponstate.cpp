#include "playerweaponstate.h"


namespace TankGame
{
	const int ammoCapacities[SPECIAL_WEAPONS_COUNT] = 
	{
		3
	};
	
	const std::string weaponDisplayNames[SPECIAL_WEAPONS_COUNT] = 
	{
		"Rocket Launcher"
	};
	
	const std::string ammoDisplayNames[SPECIAL_WEAPONS_COUNT] = 
	{
		"Rockets"
	};
	
	int GetAmmoCapacity(SpecialWeapons weapon)
	{
		return ammoCapacities[static_cast<int>(weapon)];
	}
	
	const std::string& GetWeaponDisplayName(SpecialWeapons weapon)
	{
		return weaponDisplayNames[static_cast<int>(weapon)];
	}
	
	const std::string& GetAmmoDisplayName(SpecialWeapons weapon)
	{
		return ammoDisplayNames[static_cast<int>(weapon)];
	}
	
	PlayerWeaponState::PlayerWeaponState()
	{
		std::fill(m_ammoCounts.begin(), m_ammoCounts.end(), 0);
	}
	
	int PlayerWeaponState::GetAmmoCount(SpecialWeapons weapon) const
	{
		return m_ammoCounts[static_cast<int>(weapon)];
	}
	
	int PlayerWeaponState::GiveAmmo(SpecialWeapons weapon, int amount)
	{
		int oldAmmo = GetAmmoCount(weapon);
		
		m_ammoCounts[static_cast<int>(weapon)] = glm::clamp(oldAmmo + amount, 0, GetAmmoCapacity(weapon));
		
		return m_ammoCounts[static_cast<int>(weapon)] - oldAmmo;
	}
}
