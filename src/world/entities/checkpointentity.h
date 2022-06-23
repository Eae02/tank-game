#pragma once

#include "../lights/raylightentity.h"

namespace TankGame
{
	class CheckpointEntity : public RayLightEntity, public Entity::IUpdateable
	{
	public:
		explicit CheckpointEntity(int checkpointIndex = 0);
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual void OnSpawned(class GameWorld& gameWorld) override;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		
		virtual bool CanMoveDuringUpdate() const override { return false; }
		
		inline int GetCheckpointIndex() const
		{ return m_checkpointIndex; }
		inline void SetCheckpointIndex(int checkpointIndex)
		{ m_checkpointIndex = checkpointIndex; }
		
		glm::vec2 GetCenterPos() const;
		
	private:
		int m_checkpointIndex = 0;
		
		bool m_activated = false;
		
		const class PlayerEntity* m_playerEntity = nullptr;
	};
}
