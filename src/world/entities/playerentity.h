#pragma once

#include "tankentity.h"
#include "hittable.h"
#include "playerpowerupstate.h"
#include "playerweaponstate.h"
#include "../entityhandle.h"
#include "../lights/spotlightentity.h"
#include "../../graphics/gl/texture2d.h"
#include "../../graphics/spritematerial.h"

#include <random>

namespace TankGame
{
	class PlayerEntity : public TankEntity
	{
	public:
		PlayerEntity();
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual Hittable* AsHittable() final override
		{ return this; }
		
		virtual Circle GetHitCircle() const;
		
		inline glm::vec2 GetVelocity() const
		{ return m_velocity; }
		
		Rectangle GetInteractRectangle() const;
		
		virtual CollidableTypes GetCollidableType() const override;
		
		virtual void SetHp(float hp) override;
		
		virtual const char* GetObjectName() const override;
		virtual void RenderProperties() override;
		
		inline float GetEnergy() const
		{ return m_energy; }
		
		void GiveEnergy();
		
		inline void GivePowerUp(PowerUps powerUp)
		{ m_powerUpState.GivePowerUp(powerUp); }
		inline bool HasPowerUp(PowerUps powerUp) const
		{ return m_powerUpState.HasPowerUp(powerUp); }
		
		inline const PlayerWeaponState& GetWeaponState() const
		{ return m_weaponState; }
		inline PlayerWeaponState& GetWeaponState()
		{ return m_weaponState; }
		
		static constexpr float MAX_ENERGY = 100.0f;
		
	protected:
		virtual const class SpriteMaterial& GetBaseMaterial(int frame) const override;
		virtual const class SpriteMaterial& GetCannonMaterial() const override;
		
		virtual void OnKilled() override;
		
	private:
		static bool s_areTexturesLoaded;
		static std::unique_ptr<Texture2D> s_cannonTexture;
		static std::unique_ptr<Texture2D> s_cannonNormalMap;
		static std::unique_ptr<SpriteMaterial> s_cannonMaterial;
		
		void FireSelectedWeapon(float gameTime);
		
		AudioSource m_noAmmoAudioSource;
		
		PlayerPowerUpState m_powerUpState;
		PlayerWeaponState m_weaponState;
		
		float m_energy = MAX_ENERGY;
		float m_energyRegenTime = 0;
		
		float m_rotationVelocity = 0;
		glm::vec2 m_velocity;
		
		std::normal_distribution<float> m_dist;
	};
}
