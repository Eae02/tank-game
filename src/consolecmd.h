#pragma once

#include "game.h"

namespace TankGame
{
	Level CommandCallbackLevelFromName(const std::string& name, GameWorld::Types worldType);
	
	void AddConsoleCommands(Console& console, const std::shared_ptr<Game>& game);
}
