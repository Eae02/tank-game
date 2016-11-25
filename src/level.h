#pragma once

#include "utils/filesystem.h"
#include "world/gameworld.h"

#include <string>
#include <memory>

namespace TankGame
{
	class Level : private GameWorld::IEventListener
	{
	public:
		explicit Level(std::istream& stream, GameWorld::Types worldType = GameWorld::Types::Game);
		
		Level(Level&& other);
		Level& operator=(Level&& other);
		
		static Level FromName(const std::string& name, GameWorld::Types worldType = GameWorld::Types::Game);
		
		inline GameWorld& GetGameWorld() const
		{ return *m_gameWorld; }
		
		inline class PlayerEntity& GetPlayerEntity()
		{ return *m_playerEntity; }
		inline const class PlayerEntity& GetPlayerEntity() const
		{ return *m_playerEntity; }
		
		const class CheckpointEntity* GetCheckpointFromIndex(int index) const;
		bool TryJumpToCheckpoint(int index);
		
		virtual void Update(const class UpdateInfo& updateInfo);
		
		static const fs::path& GetLevelsPath();
		
	private:
		explicit Level(struct ParseResult&& parseResult);
		
		virtual void HandleEvent(const std::string& event, Entity* sender) final override;
		
		//Needs to be a unique_ptr so the game world doesn't move in memory
		std::unique_ptr<GameWorld> m_gameWorld;
		
		class PlayerEntity* m_playerEntity;
	};
}
