#pragma once

#include "hittable.h"
#include "../lights/spotlightentity.h"
#include "../entity.h"
#include "../../audio/audiosource.h"

namespace TankGame
{
	class TankEntity : public SpotLightEntity, public Hittable, public Entity::IUpdateable, public Entity::ISpriteDrawable
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
		
		inline void SetCannonRotation(float rotation)
		{ m_cannonTransform.SetRotation(rotation); }
		
		bool CanFire(float gameTime) const;
		void FirePlasmaGun(glm::vec3 bulletColor, float damage, float gameTime, float rotationOffset = 0.0f);
		void FireRocket(float damage, float gameTime);
		
		inline void AdvanceFrame(float delta)
		{ m_frame += delta; }
		
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		virtual const Entity::ISpriteDrawable* AsSpriteDrawable() const final override
		{ return this; }
		
		virtual const char* GetSerializeClassName() const override
		{ return nullptr; }
		virtual nlohmann::json Serialize() const override;
		
		virtual IntersectInfo GetIntersectInfo(const Circle& circle) const override;
		
		virtual void RenderProperties() override;
		
	protected:
		TankEntity(glm::vec3 spotlightColor, const TextureInfo& textureInfo, int teamID, float maxHp);
		
		virtual const class SpriteMaterial& GetBaseMaterial(int frame) const = 0;
		virtual const class SpriteMaterial& GetCannonMaterial() const = 0;
		
		inline void SetFireCooldown(float fireCooldown)
		{ m_fireCooldown = fireCooldown; }
		
		static Transform GetBaseCannonTransform(const TextureInfo& textureInfo);
		
	private:
		void Fire(std::unique_ptr<Entity>&& bullet, float gameTime, float rotationOffset);
		
		AudioSource m_audioSource;
		
		TextureInfo m_textureInfo;
		
		int m_teamID;
		
		float m_frame = 0;
		
		float m_lastFireTime = 0;
		float m_fireCooldown = 0;
		float m_cannonOffset = 0;
		
		mutable Transform m_cannonTransform;
	};
}
