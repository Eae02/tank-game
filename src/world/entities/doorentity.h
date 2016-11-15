#pragma once

#include "propentity.h"

namespace TankGame
{
	class DoorEntity : public PropEntity//, public Entity::IUpdateable
	{
	public:
		inline DoorEntity() : DoorEntity("") { }
		
		explicit DoorEntity(std::string openEventName);
		
		//virtual void Update(const class UpdateInfo& updateInfo) override;
		
		//virtual Entity::IUpdateable* AsUpdatable() override;
		
		virtual void OnSpawned(class GameWorld& gameWorld) override;
		
		virtual void HandleEvent(const std::string& event, Entity* sender) override;
		
		void Open();
		
		virtual void RenderProperties() override;
		virtual const char*GetObjectName() const override;
		
		virtual nlohmann::json Serialize() const override;
		virtual const char* GetSerializeClassName() const override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
	private:
		std::string m_openEventName;
	};
}
