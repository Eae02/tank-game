#pragma once

#include "../hittable.h"
#include "../../icollidable.h"
#include "../../entityhandle.h"
#include "../../lights/raylightentity.h"
#include "../../../editor/ieditoruientity.h"
#include "../../../audio/audiosource.h"
#include "../../../graphics/spritematerial.h"

namespace TankGame
{
	class RocketTurret : public RayLightEntity, public ICollidable, public Entity::ISpriteDrawable, 
	        public Entity::IUpdateable, public Hittable, public IEditorUIEntity
	{
	public:
		RocketTurret(float minRotationAngle, float maxRotationAngle, float rotationSpeed = 1.0f,
		             float fireDelay = 2.0f);
		
		virtual void Draw(class SpriteRenderList& spriteRenderList) const override;
		
		virtual void DrawEditorUI(class UIRenderer& uiRenderer, const glm::mat3& viewTransform) const override;
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual void OnSpawned(class GameWorld& gameWorld) override;
		
		virtual ColliderInfo GetColliderInfo() const override;
		virtual CollidableTypes GetCollidableType() const override;
		
		virtual const Entity::ISpriteDrawable* AsSpriteDrawable() const final override
		{ return this; }
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		virtual Hittable* AsHittable() final override
		{ return this; }
		virtual const ICollidable* AsCollidable() const final override
		{ return this; }
		
		virtual Circle GetHitCircle() const override;
		
		virtual void EditorMoved() override;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
	protected:
		virtual void OnKilled() override;
		
	private:
		bool ShouldFire() const;
		
		void RotateTowardsPlayer(float dt);
		
		float CalcLength(const glm::vec2* forward) const;
		
		float m_minRotationAngle;
		float m_maxRotationAngle;
		float m_rotationSpeed;
		
		float m_fireDelay;
		
		double m_nextFireTime = 0;
		
		float m_firePreparationTime = 0;
		
		float m_baseRotation = 0;
		
		const class PropClass& m_cannonPropClass;
		
		const class PlayerEntity* m_playerEntity;
		
		AudioSource m_ambienceSource;
		AudioSource m_detectedSource;
		
		bool m_spinDirection = true;
	};
}
