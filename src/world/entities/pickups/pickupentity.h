#pragma once

#include "../propentity.h"
#include "../../lights/pointlightentity.h"

namespace TankGame
{
	class PickupEntity : public PointLightEntity, public PropEntity, public Entity::IUpdateable
	{
	public:
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		
		inline void SetVelocity(glm::vec2 velocity)
		{ m_velocity = velocity; }
		
		virtual void OnSpawned(GameWorld& gameWorld) override;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
		virtual const char* GetSerializeClassName() const override
		{ return nullptr; }
		virtual nlohmann::json Serialize() const override
		{ return { }; }
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
		virtual bool CanMoveDuringUpdate() const override { return true; }
		
	protected:
		PickupEntity(const std::string& propClassName, glm::vec3 glowColor);
		
		virtual void OnPickedUp(class PlayerEntity& playerEntity) = 0;
		
	private:
		glm::vec2 m_velocity;
		class PlayerEntity* m_playerEntity;
	};
}
