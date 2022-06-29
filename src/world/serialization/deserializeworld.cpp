#include "deserializeworld.h"
#include "parseutils.h"
#include "sections.h"
#include "entityparsers/entityparser.h"
#include "../entities/playerentity.h"
#include "../../utils/jsonparseutils.h"

#include <nlohmann/json.hpp>

namespace TankGame
{
	static void ParseEntity(GameWorld& gameWorld, const nlohmann::json& entityElement)
	{
		auto classIt = entityElement.find("class");
		if (classIt == entityElement.end())
			return;
		
		std::string entityClass = classIt->get<std::string>();
		
		const EntityParser* parser = GetEntityParser(entityClass);
		if (parser != nullptr)
		{
			const EntityParser::ParseParams params = { &gameWorld.GetParticlesManager() };
			
			std::unique_ptr<Entity> entity = parser->Parse(entityElement, params);
			
			auto nameIt = entityElement.find("name");
			if (nameIt != entityElement.end())
				entity->SetName(*nameIt);
			
			gameWorld.Spawn(std::move(entity));
		}
	}
	
	std::unique_ptr<GameWorld> DeserializeWorld(std::istream& stream, GameWorld::Types type)
	{
		char magicNum[4];
		stream.read(magicNum, sizeof(magicNum));
		if (memcmp(magicNum, "lvl0", 4) != 0)
			return nullptr;
		
		std::vector<char> jsonSection = ReadSection(stream);
		nlohmann::json json = nlohmann::json::parse(jsonSection.begin(), jsonSection.end());
		
		std::vector<char> tilesSection = ReadSection(stream);
		const uint8_t* tileIds = reinterpret_cast<const uint8_t*>(tilesSection.data());
		
		uint32_t width = json["width"].get<uint32_t>();
		uint32_t height = json["height"].get<uint32_t>();
		
		std::unique_ptr<GameWorld> gameWorld = std::make_unique<GameWorld>(width, height, type);
		
		//Parses the tile grid
		std::unique_ptr<TileGrid> tileGrid = std::make_unique<TileGrid>(width, height);
		for (uint32_t y = 0; y < height; y++)
		{
			for (uint32_t x = 0; x < width; x++)
				tileGrid->SetTileID({ x, y }, tileIds[x + y * width]);
		}
		tileGrid->UploadGridData();
		gameWorld->SetTileGrid(std::move(tileGrid));
		
		//Spawns the player entity
		if (type != GameWorld::Types::MenuBackground)
		{
			std::unique_ptr<PlayerEntity> playerEntity = std::make_unique<PlayerEntity>();
			
			ParseTransform(json["initial_transform"], playerEntity->GetTransform());
			gameWorld->SetCheckpoint(-1, playerEntity->GetTransform().GetPosition(),
			                         playerEntity->GetTransform().GetRotation());
			
			playerEntity->SetName("player");
			
			gameWorld->SetFocusEntity(gameWorld->Spawn(std::move(playerEntity)).Get());
		}
		
		//Parses and spawns entities
		for (auto& entityElement : json["entities"])
			ParseEntity(*gameWorld, entityElement);
		
		return gameWorld;
	}
}
