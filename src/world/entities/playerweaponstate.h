#pragma once

namespace TankGame
{
	enum class SpecialWeapons
	{
		RocketLauncher = 0
	};
	
	constexpr int SPECIAL_WEAPONS_COUNT = 1;
	
	int GetAmmoCapacity(SpecialWeapons weapon);
	const std::string& GetWeaponDisplayName(SpecialWeapons weapon);
	const std::string& GetAmmoDisplayName(SpecialWeapons weapon);
	
	class PlayerWeaponState
	{
	public:
		PlayerWeaponState();
		
		int GetAmmoCount(SpecialWeapons weapon) const;
		
		/*
			Tries to gives the player ammunition for a certain weapon, returns how much ammunition was actually given.
			Can be called with negative amount to take away ammunition.
		*/
		int GiveAmmo(SpecialWeapons weapon, int amount);
		
		inline bool IsUsingPlasmaGun() const
		{ return m_usingPlasmaGun; }
		inline SpecialWeapons GetCurrentSpecialWeapon() const
		{ return m_currentSpecialWeapon; }
		
		inline void SelectPlasmaGun()
		{
			m_usingPlasmaGun = true;
		}
		
		inline void SelectSpecialWeapon(SpecialWeapons specialWeapon)
		{
			m_usingPlasmaGun = false;
			m_currentSpecialWeapon = specialWeapon;
		}
		
	private:
		bool m_usingPlasmaGun = true;
		SpecialWeapons m_currentSpecialWeapon = SpecialWeapons::RocketLauncher;
		
		std::array<int, SPECIAL_WEAPONS_COUNT> m_ammoCounts;
	};
}
