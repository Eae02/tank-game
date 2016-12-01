#pragma once

#include "propentity.h"
#include "playerweaponstate.h"
#include "../entityhandle.h"
#include "../../audio/audiosource.h"

namespace TankGame
{
	class AmmoBoxEntity : public PropEntity
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
		
	private:
		PlayerWeaponState* GetWeaponState();
		
		std::array<bool, SPECIAL_WEAPONS_COUNT> m_hasAmmoType;
		
		AudioSource m_audioSource;
		
		EntityHandle m_playerHandle;
	};
}
