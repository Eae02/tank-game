#pragma once

#include "../lights/raylightentity.h"
#include "../icollidable.h"

namespace TankGame
{
	class DeflectionFieldEntity : public RayLightEntity, public ICollidable, public Entity::IUpdateable
	{
	public:
		explicit DeflectionFieldEntity(float length = 1);
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		inline void SetDeflectEnemy(bool deflectEnemy)
		{ m_deflectEnemy = deflectEnemy; }
		inline bool DeflectEnemy() const
		{ return m_deflectEnemy; }
		
		inline void Flash()
		{ m_flashTime = 1; }
		
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		
		virtual const ICollidable* AsCollidable() const final override
		{ return this; }
		
		virtual bool CanMoveDuringUpdate() const override { return false; }
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		
		virtual ColliderInfo GetColliderInfo() const override;
		virtual CollidableTypes GetCollidableType() const override;
		virtual bool IsStaticCollider() const override;
		
	private:
		bool m_deflectEnemy = false;
		
		float m_flashTime = 0;
	};
}
