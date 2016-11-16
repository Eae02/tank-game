#pragma once

#include "propentity.h"

namespace TankGame
{
	class DoorEntity : public PropEntity//, public Entity::IUpdateable
	{
	public:
		inline DoorEntity() : DoorEntity(nullptr, 0) { }
		
		DoorEntity(const std::string* openEvents, size_t numOpenEvents);
		
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
		struct OpenEvent
		{
			std::string m_name;
			bool m_received = false;
			
			OpenEvent() = default;
			
			inline explicit OpenEvent(std::string name)
			    : m_name(std::move(name)) { }
		};
		
		std::vector<OpenEvent> m_openEvents;
		
		int m_editorCurrentOpenEvent = 0;
	};
}
