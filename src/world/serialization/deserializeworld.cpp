#include "deserializeworld.h"
#include "parseutils.h"
#include "entityparsers/entityparser.h"
#include "../entities/playerentity.h"
#include "../../utils/jsonparseutils.h"

#include <zlib.h>
#include <algorithm>
#include <iostream>

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
			std::unique_ptr<Entity> entity = parser->Parse(entityElement);
			
			auto nameIt = entityElement.find("name");
			if (nameIt != entityElement.end())
				gameWorld.SpawnNamed(std::move(entity), nameIt->get<std::string>());
			else
				gameWorld.Spawn(std::move(entity));
		}
	}
	
	static std::vector<char> ReadSection(std::istream& input)
	{
		uint64_t sectionSize;
		uint64_t compressedSectionSize;
		
		input.read(reinterpret_cast<char*>(&sectionSize), sizeof(sectionSize));
		input.read(reinterpret_cast<char*>(&compressedSectionSize), sizeof(compressedSectionSize));
		
		std::vector<char> output;
		output.reserve(sectionSize);
		
		z_stream inflateStream = { };
		inflateInit(&inflateStream);
		
		int status;
		char outBuffer[256];
		char inBuffer[256];
		
		long bytesLeft = compressedSectionSize;
		
		//Inflates the data 256 bytes at a time
		do
		{
			long bytesToRead = std::min<long>(sizeof(inBuffer), bytesLeft);
			input.read(inBuffer, bytesToRead);
			
			assert(input.gcount() == bytesToRead);
			
			bytesLeft -= bytesToRead;
			
			inflateStream.avail_in = bytesToRead;
			inflateStream.next_in = reinterpret_cast<Bytef*>(inBuffer);
			
			if (inflateStream.avail_in == 0)
				break;
			
			do
			{
				inflateStream.avail_out = sizeof(outBuffer);
				inflateStream.next_out = reinterpret_cast<Bytef*>(outBuffer);
				
				status = inflate(&inflateStream, Z_NO_FLUSH);
				assert(status != Z_STREAM_ERROR);
				
				if (status == Z_MEM_ERROR)
					throw std::bad_alloc();
				if (status == Z_DATA_ERROR || status == Z_NEED_DICT)
					throw std::runtime_error("Invalid deflate stream.");
				
				int bytesCompressed = static_cast<int>(sizeof(outBuffer)) - inflateStream.avail_out;
				for (int i = 0; i < bytesCompressed; i++)
					output.push_back(outBuffer[i]);
			}
			while (inflateStream.avail_out == 0);
		}
		while (status != Z_STREAM_END && bytesLeft > 0);
		
		inflateEnd(&inflateStream);
		
		return output;
	}
	
	std::unique_ptr<GameWorld> DeserializeWorld(std::istream& stream, GameWorld::Types type)
	{
		char magicNum[4];
		stream.read(magicNum, sizeof(magicNum));
		if (memcmp(magicNum, "lvl0", 4) != 0)
			throw std::runtime_error("Invalid level file.");
		
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
				tileGrid->SetTileID(x, y, tileIds[x + y * width]);
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
			
			gameWorld->SetFocusEntity(gameWorld->SpawnNamed(std::move(playerEntity), "player").Get());
		}
		
		//Parses and spawns entities
		for (auto& entityElement : json["entities"])
			ParseEntity(*gameWorld, entityElement);
		
		return gameWorld;
	}
}
