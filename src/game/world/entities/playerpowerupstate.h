#pragma once

#include <string>

namespace TankGame
{
	enum class PowerUps
	{
		HomingBullets = 0
	};
	
	constexpr int POWER_UP_COUNT = 1;
	
	std::u32string GetPowerUpName(PowerUps powerUp);
	float GetPowerUpTime(PowerUps powerUp);
	
	class PlayerPowerUpState
	{
	public:
		PlayerPowerUpState();
		
		void Update(float dt);
		
		inline void GivePowerUp(PowerUps powerUp)
		{
			m_powerUpTimes[static_cast<int>(powerUp)] = GetPowerUpTime(powerUp);
		}
		
		inline bool HasPowerUp(PowerUps powerUp) const
		{
			return m_powerUpTimes[static_cast<int>(powerUp)] > 0;
		}
		
	private:
		float m_powerUpTimes[POWER_UP_COUNT];
	};
}
