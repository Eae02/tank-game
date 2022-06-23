#pragma once

#include "../lights/pointlightentity.h"
#include "../particles/systems/energyballparticlesystem.h"
#include "particlesystementity.h"
#include "hittable.h"

#include <glm/glm.hpp>

namespace TankGame
{
	class EnergyBall : public PointLightEntity, public Entity::IUpdateable, Entity::IDistortionDrawable,
	        public ParticleSystemEntity<EnergyBallParticleSystem>, public Hittable
	{
	public:
		EnergyBall(glm::vec2 direction, float damage, TankGame::ParticlesManager& particlesManager);
		
		inline void SetDamage(float damage)
		{ m_damage = damage; }
		inline float GetDamage() const
		{ return m_damage; }
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual Circle GetHitCircle() const final override;
		
		virtual bool CanMoveDuringUpdate() const override { return true; }
		
		virtual void DrawDistortions() const override;
		
		virtual const Entity::IDistortionDrawable* AsDistortionDrawable() const final override
		{ return this; }
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		virtual Hittable* AsHittable() final override
		{ return this; }
		
		virtual void OnSpawned(class GameWorld& gameWorld) override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		virtual std::unique_ptr<Entity> Clone() const override;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
	private:
		float m_damage = 70;
		
		glm::vec2 m_direction;
		float m_directionAngle;
	};
}
