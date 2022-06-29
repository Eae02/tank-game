#pragma once

#include "../../rectangle.h"
#include "../../world/entity.h"
#include "../../world/serialization/entityparsers/entityparser.h"

#include <nlohmann/json.hpp>

namespace TankGame
{
	class EntitySpawnMenu
	{
	public:
		using EntityCreator = std::unique_ptr<Entity> (*)();
		
		EntitySpawnMenu();
		
		inline void SetGameWorld(class GameWorld* gameWorld)
		{ m_gameWorld = gameWorld; }
		
		void Show(glm::vec2 spawnPosition);
		
		void Render();
		
	private:
		struct EntityEntry
		{
			std::string m_label;
			const EntityParser* m_entityParser;
			nlohmann::json m_jsonElement;
			
			EntityEntry(std::string label, const nlohmann::json& jsonElement);
		};
		
		bool m_setKeyboardFocus = false;
		
		std::array<char, 256> m_searchBuffer;
		
		glm::vec2 m_spawnPosition;
		
		class GameWorld* m_gameWorld = nullptr;
		
		std::vector<EntityEntry> m_entries;
	};
}
