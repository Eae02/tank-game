#pragma once

#include "projectileentity.h"
#include "../particlesystementity.h"
#include "../../lights/pointlightentity.h"
#include "../../particles/systems/smokeparticlesystem.h"
#include "../../../graphics/gl/texture2d.h"
#include "../../../graphics/spritematerial.h"
#include "../../../audio/audiosource.h"


namespace TankGame
{
	class RocketEntity : public PointLightEntity, public ProjectileEntity,
	        public ParticleSystemEntity<SmokeParticleSystem>, public Entity::ISpriteDrawable
	{
	public:
		RocketEntity(ParticlesManager& particlesManager, int teamID, const Entity* sourceEntity, float damage);
		
		virtual void Draw(class SpriteRenderList& spriteRenderList) const override;
		
		virtual const Entity::ISpriteDrawable* AsSpriteDrawable() const final override
		{ return this; }
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual void OnSpawned(class GameWorld& gameWorld) override;
		
		virtual const char* GetSerializeClassName() const override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
	protected:
		virtual void OnImpact(ImpactFlags flags, glm::vec2 penetration) override;
		
	private:
		static std::unique_ptr<Texture2D> s_diffuse;
		static std::unique_ptr<Texture2D> s_normalMap;
		static std::unique_ptr<SpriteMaterial> s_material;
		
		mutable Transform m_spriteTransform;
		
		AudioSource m_audioSource;
	};
}
