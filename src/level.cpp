#include "level.h"
#include "world/entities/playerentity.h"
#include "world/entities/checkpointentity.h"
#include "world/serialization/deserializeworld.h"
#include "audio/almanager.h"
#include "utils/ioutils.h"

#include <fstream>

namespace TankGame
{
	static fs::path theLevelsPath;
	
	const fs::path& Level::GetLevelsPath()
	{
		if (theLevelsPath.empty())
			theLevelsPath = GetResDirectory() / "levels";
		return theLevelsPath;
	}
	
	Level::Level(std::istream& stream)
	    : m_gameWorld(DeserializeWorld(stream)),
	      m_playerEntity(dynamic_cast<PlayerEntity*>(m_gameWorld->GetEntityByName("player")))
	{
		m_gameWorld->SetEventListener(this);
	}
	
	Level::Level(Level&& other)
	    : m_gameWorld(std::move(other.m_gameWorld)), m_playerEntity(other.m_playerEntity)
	{
		m_gameWorld->SetEventListener(this);
	}
	
	Level& Level::operator=(Level&& other)
	{
		m_gameWorld = std::move(other.m_gameWorld);
		m_playerEntity = other.m_playerEntity;
		
		m_gameWorld->SetEventListener(this);
		
		return *this;
	}
	
	Level Level::FromName(const std::string& name)
	{
		fs::path fullPath = Level::GetLevelsPath() / name;
		if (!fs::exists(fullPath))
			throw std::runtime_error("Level not found: '" + name + "'.");
		
		std::string fullPathString = fullPath.string();
		
		std::ifstream stream(fullPathString, std::ios::binary);
		if (!stream)
			throw std::runtime_error("Error opening file for reading: '" + fullPathString + "'.");
		
		return Level(stream);
	}
	
	const CheckpointEntity* Level::GetCheckpointFromIndex(int index) const
	{
		const CheckpointEntity* checkpoint = nullptr;
		
		m_gameWorld->IterateEntities([&] (const Entity& entity)
		{
			const CheckpointEntity* checkpointEntity = dynamic_cast<const CheckpointEntity*>(&entity);
			if (checkpointEntity != nullptr && checkpointEntity->GetCheckpointIndex() == index)
				checkpoint = checkpointEntity;
		});
		
		return checkpoint;
	}
	
	void Level::Update(const class UpdateInfo& updateInfo)
	{
		m_gameWorld->Update(updateInfo);
		
		UpdateListener(*m_playerEntity);
	}
	
	void Level::HandleEvent(const std::string& event, Entity* sender)
	{
		if (event == "EndLevel")
		{
			
		}
		else if (event == "PlayerKilled")
		{
			m_playerEntity->GetTransform().SetPosition(m_gameWorld->GetRespawnPosition());
			m_playerEntity->GetTransform().SetRotation(m_gameWorld->GetRespawnRotation());
			m_playerEntity->SetHp(m_playerEntity->GetMaxHp());
		}
	}
}
