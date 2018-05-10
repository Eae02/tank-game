#pragma once

#include <istream>
#include <memory>
#include "../gameworld.h"

namespace TankGame
{
	enum class DeserializeFlags
	{
		Default = 0,
		EditorWorld = 1,
		MenuBackground = 2
	};
	
	inline DeserializeFlags operator|(DeserializeFlags a, DeserializeFlags b)
	{ return static_cast<DeserializeFlags>(static_cast<int>(a) | static_cast<int>(b)); }
	inline int operator&(DeserializeFlags a, DeserializeFlags b)
	{ return static_cast<int>(a) & static_cast<int>(b); }
	
	std::unique_ptr<GameWorld> DeserializeWorld(std::istream& stream, GameWorld::Types type = GameWorld::Types::Game);
}
