#include "serializeworld.h"
#include "sections.h"
#include "../gameworld.h"

#include <json.hpp>

namespace TankGame
{
	void SerializeWorld(const GameWorld& gameWorld, std::ostream& stream)
	{
		stream.write("lvl0", 4);
		
		nlohmann::json json;
		
		json["width"] = gameWorld.GetWidth();
		json["height"] = gameWorld.GetHeight();
		
		std::vector<nlohmann::json> entities;
		
		gameWorld.IterateEntities([&] (const Entity& entity)
		{
			const char* className = entity.GetSerializeClassName();
			if (className == nullptr || !entity.IsEditorVisible())
				return;
			
			nlohmann::json element = entity.Serialize();
			if (!entity.GetName().empty())
				element["name"] = entity.GetName();
			
			element["class"] = className;
			
			entities.emplace_back(std::move(element));
		});
		
		json["entities"] = std::move(entities);
		
		if (const Entity* playerEntity = gameWorld.GetEntityByName("player"))
		{
			json["initial_transform"] = playerEntity->GetTransform().Serialize(Transform::Properties::Position |
			                                                                   Transform::Properties::Rotation);
		}
		
		std::string jsonStr = json.dump();
		WriteSection(jsonStr.data(), jsonStr.size(), stream);
		
		WriteSection(reinterpret_cast<const char*>(gameWorld.GetTileGrid()->GetDataPtr()),
		             gameWorld.GetTileGrid()->GetDataSize(), stream);
	}
	
	void WriteEmptyWorld(const std::string& name, uint32_t width, uint32_t height, std::ostream& stream)
	{
		stream.write("lvl0", 4);
		
		Transform initialTransform;
		initialTransform.SetPosition({ 3, 3 });
		
		nlohmann::json json;
		
		json["width"] = width;
		json["height"] = height;
		json["name"] = name;
		json["initial_transform"] = initialTransform.Serialize(Transform::Properties::Position);
		json["entities"] = nlohmann::json::array();
		
		std::string jsonStr = json.dump();
		WriteSection(jsonStr.data(), jsonStr.size(), stream);
		
		std::vector<uint8_t> tileIDs(width * height);
		for (uint32_t y = 0; y < height; y++)
		{
			for (uint32_t x = 0; x < width; x++)
				tileIDs[x + y * width] = (x == 0 || y == 0 || x == width - 1 || y == height - 1) ? 1 : 0;
		}
		
		WriteSection(reinterpret_cast<const char*>(tileIDs.data()), tileIDs.size(), stream);
	}
}
