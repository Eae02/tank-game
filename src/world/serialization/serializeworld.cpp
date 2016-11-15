#include "serializeworld.h"
#include "../gameworld.h"

#include <zlib.h>
#include <json.hpp>

namespace TankGame
{
	void DeflateAndWrite(const char* data, size_t dataSize, std::ostream& output)
	{
		z_stream deflateStream = { };
		
		deflateStream.avail_in = dataSize;
		deflateStream.next_in = reinterpret_cast<const Bytef*>(data);
		
		if (deflateInit(&deflateStream, Z_DEFAULT_COMPRESSION) != Z_OK)
			throw std::runtime_error("Error initializing ZLIB.");
		
		char outBuffer[256];
		int status;
		
		//Deflates and writes the data 256 bytes at a time
		do
		{
			deflateStream.avail_out = sizeof(outBuffer);
			deflateStream.next_out = reinterpret_cast<Bytef*>(outBuffer);
			
			status = deflate(&deflateStream, Z_FINISH);
			assert(status != Z_STREAM_ERROR);
			
			int bytesDecompressed = sizeof(outBuffer) - deflateStream.avail_out;
			output.write(outBuffer, bytesDecompressed);
		} while (deflateStream.avail_out == 0);
		
		deflateEnd(&deflateStream);
	}
	
	void WriteSection(const char* data, size_t dataSize, std::ostream& stream)
	{
		uint64_t payloads[2] = { dataSize, 0 };
		stream.write(reinterpret_cast<const char*>(payloads), sizeof(payloads));
		
		auto dataBeginPos = stream.tellp();
		
		DeflateAndWrite(data, dataSize, stream);
		
		auto dataEndPos = stream.tellp();
		uint64_t compressedSize = dataEndPos - dataBeginPos;
		
		stream.seekp(dataBeginPos - static_cast<std::ostream::pos_type>(sizeof(uint64_t)));
		stream.write(reinterpret_cast<const char*>(&compressedSize), sizeof(compressedSize));
		stream.seekp(dataEndPos);
	}
	
	void SerializeWorld(const GameWorld& gameWorld, std::ostream& stream)
	{
		stream.write("lvl0", 4);
		
		nlohmann::json json = gameWorld.Serialize();
		
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
