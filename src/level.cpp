#include "level.h"
#include "updateinfo.h"
#include "world/entities/playerentity.h"
#include "world/entities/checkpointentity.h"
#include "world/serialization/deserializeworld.h"
#include "audio/audio.h"
#include "utils/ioutils.h"
#include "lua/luavm.h"
#include "profiling.h"

#include <fstream>

namespace TankGame
{
	static fs::path theLevelsPath;
	
	const fs::path& Level::GetLevelsPath()
	{
		if (theLevelsPath.empty())
			theLevelsPath = resDirectoryPath / "levels";
		return theLevelsPath;
	}
	
	Level::Level(std::istream& stream, GameWorld::Types worldType)
	    : m_gameWorld(DeserializeWorld(stream, worldType)),
	      m_luaSandbox(std::make_unique<Lua::Sandbox>(Lua::GetState())),
	      m_playerEntity(dynamic_cast<PlayerEntity*>(m_gameWorld->GetEntityByName("player")))
	{
		m_luaSandbox->PushTable(Lua::GetState());
		m_gameWorld->InitLuaSandbox(Lua::GetState());
		lua_pop(Lua::GetState(), 1);
		
		m_gameWorld->SetLuaSandbox(m_luaSandbox.get());
		
		m_gameWorld->IterateEntities([&] (const Entity& entity)
		{
			if (const CheckpointEntity* checkpointEntity = dynamic_cast<const CheckpointEntity*>(&entity))
				m_checkpointEntitiesByIndex.emplace(checkpointEntity->GetCheckpointIndex(), checkpointEntity);
		});
	}
	
	std::optional<Level> Level::FromName(const std::string& name, GameWorld::Types worldType)
	{
		fs::path fullPath = Level::GetLevelsPath() / name;
		std::string fullPathString = fullPath.string();
		
		std::ifstream stream(fullPathString, std::ios::binary);
		if (!stream)
			return { };
		
		Level level(stream, worldType);
		
		fs::path scriptPath = fullPath;
		scriptPath += ".lua";
		
		if (fs::exists(scriptPath))
			level.RunScript(scriptPath);
		
		return level;
	}
	
	const CheckpointEntity* Level::GetCheckpointFromIndex(int index) const
	{
		auto it = m_checkpointEntitiesByIndex.find(index);
		if (it == m_checkpointEntitiesByIndex.end())
			return nullptr;
		return it->second;
	}
	
	bool Level::TryJumpToCheckpoint(int index)
	{
		const CheckpointEntity* checkpoint = GetCheckpointFromIndex(index);
		if (checkpoint == nullptr)
			return false;
		
		glm::vec2 pos = checkpoint->GetCenterPos();
		m_playerEntity->GetTransform().SetPosition(pos);
		m_gameWorld->SetCheckpoint(index, pos, 0.0f);
		
		return true;
	}
	
	void Level::Update(const UpdateInfo& updateInfo)
	{
		FUNC_TIMER
		
		m_gameWorld->Update(updateInfo);
		
		m_luaSandbox->Update(updateInfo.m_dt);
		
		UpdateListener(*m_playerEntity);
	}
	
	void Level::RunScript(const fs::path& path)
	{
		Lua::DoString(ReadFileContents(path), m_luaSandbox.get());
	}
}
