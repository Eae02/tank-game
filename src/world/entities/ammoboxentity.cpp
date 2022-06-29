#include "ammoboxentity.h"
#include "playerentity.h"
#include "explosionentity.h"
#include "../spteams.h"
#include "../gameworld.h"
#include "../../audio/soundsmanager.h"

#include <nlohmann/json.hpp>
#include <imgui.h>

namespace TankGame
{
	AmmoBoxEntity::AmmoBoxEntity()
	    : PropEntity("AmmoBox", 0.5f, true), Hittable(50, NeutralTeamID),
	      m_audioSource(AudioSource::VolumeModes::Effect)
	{
		SetZ(0.6f);
		std::fill(m_hasAmmoType.begin(), m_hasAmmoType.end(), true);
		
		m_audioSource.SetBuffer(SoundsManager::GetInstance().GetSound("AmmoRefill"));
	}
	
	AmmoBoxEntity::AmmoBoxEntity(const AmmoBoxEntity& other)
	    : AmmoBoxEntity()
	{
		m_hasAmmoType = other.m_hasAmmoType;
	}
	
	void AmmoBoxEntity::SetHasAmmoType(SpecialWeapons weapon, bool hasType)
	{
		m_hasAmmoType[static_cast<int>(weapon)] = hasType;
	}
	
	bool AmmoBoxEntity::CanInteract()
	{
		PlayerWeaponState* weaponState = GetWeaponState();
		if (weaponState == nullptr)
			return false;
		
		for (int i = 0; i < SPECIAL_WEAPONS_COUNT; i++)
		{
			SpecialWeapons weapon = static_cast<SpecialWeapons>(i);
			if (weaponState->GetAmmoCount(weapon) < GetAmmoCapacity(weapon))
				return true;
		}
		
		return false;
	}
	
	void AmmoBoxEntity::OnInteract()
	{
		PlayerWeaponState* weaponState = GetWeaponState();
		if (weaponState == nullptr)
			return;
		
		for (int i = 0; i < SPECIAL_WEAPONS_COUNT; i++)
		{
			SpecialWeapons weapon = static_cast<SpecialWeapons>(i);
			weaponState->GiveAmmo(weapon, GetAmmoCapacity(weapon));
		}
		
		m_audioSource.SetPosition(GetTransform().GetPosition());
		m_audioSource.Play();
	}
	
	std::unique_ptr<Entity> AmmoBoxEntity::Clone() const
	{
		return std::make_unique<AmmoBoxEntity>(*this);
	}
	
	const char* AmmoBoxEntity::GetSerializeClassName() const
	{
		return "AmmoBox";
	}
	
	nlohmann::json AmmoBoxEntity::Serialize() const
	{
		nlohmann::json json;
		
		json["transform"] = GetTransform().Serialize(Transform::Properties::Position | Transform::Properties::Rotation);
		
		nlohmann::json::array_t ammoTypesEl;
		for (bool hasAmmoType : m_hasAmmoType)
			ammoTypesEl.push_back(hasAmmoType);
		json["ammoTypes"] = ammoTypesEl;
		
		return json;
	}
	
	void AmmoBoxEntity::RenderProperties()
	{
		GetTransform().RenderProperties(Transform::Properties::Position | Transform::Properties::Rotation);
		
		for (int i = 0; i < SPECIAL_WEAPONS_COUNT; i++)
		{
			std::string label = "Has " + GetWeaponDisplayName(static_cast<SpecialWeapons>(i)) + " Ammo";
			ImGui::Checkbox(label.c_str(), &m_hasAmmoType[i]);
		}
	}
	
	const char* AmmoBoxEntity::GetObjectName() const
	{
		return "Ammo Box";
	}
	
	Circle AmmoBoxEntity::GetHitCircle() const
	{
		return GetBoundingCircle();
	}
	
	PlayerWeaponState* AmmoBoxEntity::GetWeaponState()
	{
		if (!m_playerHandle.IsAlive())
		{
			const Entity* player = GetGameWorld()->GetEntityByName("player");
			if (player == nullptr)
				return nullptr;
			m_playerHandle = EntityHandle(*GetGameWorld(), *player);
		}
		
		PlayerEntity* playerEntity = dynamic_cast<PlayerEntity*>(m_playerHandle.Get());
		if (playerEntity == nullptr)
			return nullptr;
		
		return &playerEntity->GetWeaponState();
	}
	
	void AmmoBoxEntity::OnKilled()
	{
		auto explosion = std::make_unique<ExplosionEntity>(GetGameWorld()->GetParticlesManager());
		explosion->GetTransform().SetPosition(GetTransform().GetPosition());
		GetGameWorld()->Spawn(std::move(explosion));
		
		Despawn();
	}
}
