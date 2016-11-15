#pragma once

#include "../entity.h"
#include "../../editor/ieditoruientity.h"

namespace TankGame
{
	class EventBox : public virtual Entity, public Entity::IUpdateable, public IEditorUIEntity
	{
	public:
		EventBox() = default;
		
		inline EventBox(std::string enterEventName, std::string leaveEventName)
		    : m_playerEnterEvent(std::move(enterEventName)), m_playerLeaveEvent(std::move(leaveEventName)) { }
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual void OnSpawned(class GameWorld& gameWorld) override;
		
		virtual void DrawEditorUI(class UIRenderer& uiRenderer, const glm::mat3& uiViewMatrix) const override;
		
		inline void SetPlayerEnterEvent(std::string eventName)
		{ m_playerEnterEvent = std::move(eventName); }
		inline void SetPlayerLeaveEvent(std::string eventName)
		{ m_playerLeaveEvent = std::move(eventName); }
		
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
	private:
		std::string m_playerEnterEvent;
		std::string m_playerLeaveEvent;
		
		bool m_wasPlayerInBox = false;
		
		const class PlayerEntity* m_player = nullptr;
	};
}
