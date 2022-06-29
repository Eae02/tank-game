#include "levelmenuinfo.h"
#include "level.h"
#include "utils/jsonparseutils.h"
#include "world/entities/checkpointentity.h"
#include "world/entities/playerentity.h"
#include "world/serialization/sections.h"
#include "world/serialization/screenshotserializer.h"

#include <ostream>
#include <istream>
#include <nlohmann/json.hpp>

namespace TankGame
{
#ifndef __EMSCRIPTEN__
	void LevelMenuInfo::WriteMenuInfo(const Level& level, const std::string& name, std::ostream& outputStream)
	{
		outputStream.write("lmi0", 4);
		
		//Searches for checkpoints
		std::vector<const CheckpointEntity*> checkpoints;
		level.GetGameWorld().IterateEntities([&] (const Entity& entity)
		{
			const CheckpointEntity* checkpoint = dynamic_cast<const CheckpointEntity*>(&entity);
			
			if (checkpoint == nullptr)
				return;
			
			std::string keyName = std::to_string(checkpoint->GetCheckpointIndex());
			
			auto checkpointPos = std::find_if(checkpoints.begin(), checkpoints.end(), [=] (const CheckpointEntity* c)
			{
				return c->GetCheckpointIndex() == checkpoint->GetCheckpointIndex();
			});
			
			if (checkpointPos != checkpoints.end())
				GetLogStream() << LOG_ERROR << "Multple checkpoints share index " << keyName << "!\n";
			
			checkpoints.push_back(checkpoint);
		});
		
		ScreenShotSerializer screenShotSerializer(level.GetGameWorld(), (128 * 16) / 9, 128);
		nlohmann::json::array_t checkpointsJson;
		
		screenShotSerializer.WriteScreenShot(level.GetPlayerEntity().GetTransform().GetPosition());
		
		//Renders checkpoint images and builds a json object for each checkpoint
		for (size_t i = 0; i < checkpoints.size(); i++)
		{
			glm::vec2 center = checkpoints[i]->GetCenterPos();
			
			screenShotSerializer.WriteScreenShot(center);
			
			checkpointsJson.emplace_back(nlohmann::json::object_t{
				{ "index", checkpoints[i]->GetCheckpointIndex() },
				{ "image_index", i + 1 },
				{ "position", { center.x, center.y } }
			});
		}
		
		nlohmann::json json;
		json["name"] = name;
		json["checkpoints"] = std::move(checkpointsJson);
		
		std::string jsonString = json.dump();
		WriteSection(jsonString.data(), jsonString.size(), outputStream);
		
		screenShotSerializer.WriteResult(outputStream);
	}
#endif
	
	bool LevelMenuInfo::Load(std::istream& stream)
	{
		char magicNum[4];
		stream.read(magicNum, sizeof(magicNum));
		if (memcmp(magicNum, "lmi0", 4) != 0)
			return false;
		
		std::vector<char> jsonSection = ReadSection(stream);
		nlohmann::json json = nlohmann::json::parse(jsonSection);
		
		m_name = json["name"];
		
		//Parses checkpoint json objects
		auto checkpointsArray = json["checkpoints"];
		m_checkpoints.resize(checkpointsArray.size());
		for (size_t i = 0; i < m_checkpoints.size(); i++)
		{
			m_checkpoints[i].m_checkpointIndex = checkpointsArray[i]["index"].get<int>();
			m_checkpoints[i].m_imageIndex = checkpointsArray[i]["image_index"].get<size_t>();
			m_checkpoints[i].m_position = ParseVec2(checkpointsArray[i]["position"]);
		}
		
		//Sorts the checkpoints by their checkpoint indices
		std::sort(m_checkpoints.begin(), m_checkpoints.end(), [] (const Checkpoint& a, const Checkpoint& b)
		{
			return a.m_checkpointIndex < b.m_checkpointIndex;
		});
		
		//Reads checkpoint images
		m_checkpointImages = ScreenShotSerializer::Deserialize(stream);
		
		return true;
	}
}
