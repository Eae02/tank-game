#pragma once

#include "world/gameworld.h"
#include "lua/sandbox.h"

namespace TankGame
{
	class Level
	{
	public:
		explicit Level(std::istream& stream, GameWorld::Types worldType = GameWorld::Types::Game);
		
		static std::optional<Level> FromName(const std::string& name, GameWorld::Types worldType = GameWorld::Types::Game);
		
		inline GameWorld& GetGameWorld() const
		{ return *m_gameWorld; }
		
		inline class PlayerEntity& GetPlayerEntity()
		{ return *m_playerEntity; }
		inline const class PlayerEntity& GetPlayerEntity() const
		{ return *m_playerEntity; }
		
		const class CheckpointEntity* GetCheckpointFromIndex(int index) const;
		bool TryJumpToCheckpoint(int index);
		
		void Update(const class UpdateInfo& updateInfo);
		
		void RunScript(const fs::path& path);
		
		static const fs::path& GetLevelsPath();
		
	private:
		explicit Level(struct ParseResult&& parseResult);
		
		//Needs to be a unique_ptr so the game world doesn't move in memory
		std::unique_ptr<GameWorld> m_gameWorld;
		
		//Needs to be a unique_ptr so the sandbox doesn't move in memory
		std::unique_ptr<Lua::Sandbox> m_luaSandbox;
		
		class PlayerEntity* m_playerEntity;
	};
}
