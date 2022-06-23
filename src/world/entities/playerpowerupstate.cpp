#include "playerpowerupstate.h"

#include <algorithm>
#include <iterator>

namespace TankGame
{
	float GetPowerUpTime(PowerUps powerUp)
	{
		switch (powerUp)
		{
		case PowerUps::HomingBullets:
			return 20;
		default:
			return 0;
		}
	}
	
	std::string GetPowerUpName(PowerUps powerUp)
	{
		switch (powerUp)
		{
		case PowerUps::HomingBullets:
			return "Homing Bullets";
		default:
			return "";
		}
	}
	
	PlayerPowerUpState::PlayerPowerUpState()
	{
		std::fill(std::begin(m_powerUpTimes), std::end(m_powerUpTimes), 0.0f);
	}
	
	void PlayerPowerUpState::Update(float dt)
	{
		for (float& powerUpTime : m_powerUpTimes)
			powerUpTime = std::max(powerUpTime - dt, 0.0f);
	}
	
	
}
