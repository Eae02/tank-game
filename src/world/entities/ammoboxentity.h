#pragma once

#include "propentity.h"
#include "playerweaponstate.h"
#include "hittable.h"
#include "../entityhandle.h"
#include "../../audio/audiosource.h"

namespace TankGame
{
	class AmmoBoxEntity : public PropEntity, public Hittable
	{
	public:
		AmmoBoxEntity();
		
		AmmoBoxEntity(const AmmoBoxEntity& other);
		
		void SetHasAmmoType(SpecialWeapons weapon, bool hasType);
		
		virtual bool CanInteract() override;
		virtual void OnInteract() override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
		virtual Circle GetHitCircle() const override;
		
		virtual Hittable* AsHittable() override
		{ return this; }
		
	protected:
		virtual void OnKilled() override;
		
	private:
		PlayerWeaponState* GetWeaponState();
		
		std::array<bool, SPECIAL_WEAPONS_COUNT> m_hasAmmoType;
		
		AudioSource m_audioSource;
		
		EntityHandle<class PlayerEntity> m_playerHandle;
	};
}
