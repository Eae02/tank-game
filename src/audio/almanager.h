#pragma once

#include <glm/glm.hpp>

namespace TankGame
{
	void InitOpenAL();
	void CloseOpenAL();
	
	void SetMasterVolume(float volume);
	
	void UpdateListener(const class PlayerEntity& playerEntity);
}
