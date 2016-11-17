#include "checkpointentity.h"
#include "playerentity.h"
#include "../gameworld.h"
#include "../../graphics/spriterenderlist.h"
#include "../../updateinfo.h"
#include "../../orientedrectangle.h"

#include <algorithm>
#include <imgui.h>

namespace TankGame
{
	static const glm::vec3 DEFAULT_COLOR = ParseColorHexCodeSRGB(0xE38D14);
	static const glm::vec3 ACTIVATED_COLOR = ParseColorHexCodeSRGB(0x49DE5D);
	
	CheckpointEntity::CheckpointEntity(int checkpointIndex)
	    : RayLightEntity(DEFAULT_COLOR, 5, Attenuation(0, 10), 1, 0.05f), m_checkpointIndex(checkpointIndex)
	{
		
	}
	
	void CheckpointEntity::Draw(SpriteRenderList& spriteRenderList) const
	{
		
	}
	
	void CheckpointEntity::Update(const UpdateInfo& updateInfo)
	{
		if (m_playerEntity == nullptr || m_activated)
			return;
		
		glm::vec2 forward = GetTransform().GetForward();
		glm::vec2 center = GetTransform().GetPosition() + forward * GetLength() * 0.5f;
		
		OrientedRectangle rectangle(center, glm::vec2(0.5f, GetLength()), GetTransform().GetRotation());
		
		if (rectangle.GetIntersectInfo(m_playerEntity->GetHitCircle()).m_intersects)
		{
			m_activated = true;
			SetColor(ACTIVATED_COLOR);
			
			GetGameWorld()->SetCheckpoint(m_checkpointIndex, center, m_playerEntity->GetTransform().GetRotation());
		}
	}
	
	void CheckpointEntity::OnSpawned(GameWorld& gameWorld)
	{
		if (gameWorld.GetWorldType() == GameWorld::Types::Game)
		{
			m_playerEntity = dynamic_cast<const PlayerEntity*>(gameWorld.GetEntityByName("player"));
		}
		
		if (gameWorld.GetWorldType() == GameWorld::Types::Editor)
		{
			std::vector<const CheckpointEntity*> checkpoints;
			
			//Finds all already spawned checkpoint entities and stores them in checkpoints
			gameWorld.IterateEntities([&] (const class Entity& entity)
			{
				const CheckpointEntity* checkPoint = dynamic_cast<const CheckpointEntity*>(&entity);
				if (checkPoint != nullptr && checkPoint != this)
					checkpoints.push_back(checkPoint);
			});
			
			//Increments m_checkpointIndex until there are no other checkpoints with the same index
			while (true)
			{
				if (std::find_if(checkpoints.begin(), checkpoints.end(), [this] (const CheckpointEntity* checkpoint)
				    { return checkpoint->m_checkpointIndex == m_checkpointIndex; }) == checkpoints.end())
				{
					break;
				}
				
				m_checkpointIndex++;
			}
		}
		
		RayLightEntity::OnSpawned(gameWorld);
	}
	
	void CheckpointEntity::RenderProperties()
	{
		RenderTransformProperty(Transform::Properties::Position | Transform::Properties::Rotation);
		
		float length = GetLength();
		if (ImGui::InputFloat("Length", &length))
			SetLength(glm::max(length, 0.0f));
		
		if (ImGui::InputInt("Index", &m_checkpointIndex))
			m_checkpointIndex = glm::max(m_checkpointIndex, 0);
	}
	
	const char* CheckpointEntity::GetObjectName() const
	{
		return "Checkpoint";
	}
	
	const char* CheckpointEntity::GetSerializeClassName() const
	{
		return "Checkpoint";
	}
	
	nlohmann::json CheckpointEntity::Serialize() const
	{
		nlohmann::json json;
		
		json["transform"] = GetTransform().Serialize(Transform::Properties::Position | Transform::Properties::Rotation);
		
		json["length"] = GetLength();
		json["index"] = m_checkpointIndex;
		
		return json;
	}
}
