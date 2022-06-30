#pragma once

#include "hittable.h"
#include "../icollidable.h"
#include "../lights/spotlightentity.h"
#include "../entity.h"
#include "../entityhandle.h"
#include "../../audio/audiosource.h"

namespace TankGame
{
	class TankEntity : public SpotLightEntity, public Hittable, public ICollidable,
	        public Entity::IUpdateable, public Entity::ISpriteDrawable
	{
	public:
		struct TextureInfo
		{
			int m_baseTextureWidth;
			int m_baseTextureHeight;
			int m_baseTextureFrames;
			int m_cannonTextureWidth;
			int m_cannonTextureHeight;
			float m_cannonYOffset;
		};
		
		virtual void Draw(class SpriteRenderList& spriteRenderList) const override;
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual void OnDespawning() override;
		
		inline void SetCannonRotation(float rotation)
		{ m_cannonTransform.SetRotation(rotation); }
		
		struct FireParameters
		{
			float m_rotationOffset = 0;
			bool m_homing = false;
		};
		
		bool CanFire(float gameTime) const;
		void FirePlasmaGun(glm::vec3 bulletColor, float damage, float gameTime, const FireParameters& params);
		void FireRocket(float damage, float gameTime, const FireParameters& params);
		
		inline void AdvanceFrame(float delta)
		{ m_frame += delta; }
		
		void SpawnShield(float hp);
		
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		virtual const Entity::ISpriteDrawable* AsSpriteDrawable() const final override
		{ return this; }
		virtual const ICollidable* AsCollidable() const final override
		{ return this; }
		
		virtual const char* GetSerializeClassName() const override
		{ return nullptr; }
		virtual nlohmann::json Serialize() const override;
		
		virtual ColliderInfo GetColliderInfo() const override;
		virtual CollidableTypes GetCollidableType() const override = 0;
		virtual bool IsStaticCollider() const override;
		
		virtual void RenderProperties() override;
		
		virtual bool CanMoveDuringUpdate() const override { return true; }
		
	protected:
		TankEntity(glm::vec3 spotlightColor, const TextureInfo& textureInfo, int teamID, float maxHp);
		
		virtual const class SpriteMaterial& GetBaseMaterial(int frame) const = 0;
		virtual const class SpriteMaterial& GetCannonMaterial() const = 0;
		
		inline void SetFireCooldown(float fireCooldown)
		{ m_fireCooldown = fireCooldown; }
		
		virtual void OnKilled() override;
		
		static Transform GetBaseCannonTransform(const TextureInfo& textureInfo);
		
	private:
		void Fire(std::unique_ptr<Entity>&& bullet, float gameTime, float rotationOffset);
		
		AudioSource m_audioSource;
		
		TextureInfo m_textureInfo;
		
		EntityHandle<> m_shieldHandle;
		
		int m_teamID;
		
		float m_frame = 0;
		
		float m_lastFireTime = 0;
		float m_fireCooldown = 0;
		float m_cannonOffset = 0;
		
		mutable Transform m_cannonTransform;
	};
}
