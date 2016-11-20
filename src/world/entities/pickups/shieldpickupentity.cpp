#include "shieldpickupentity.h"
#include "../playerentity.h"
#include "../../../audio/soundeffectplayer.h"

namespace TankGame
{
	static SoundEffectPlayer pickupEffectPlayer{ "ShieldPickup" };
	
	ShieldPickupEntity::ShieldPickupEntity()
	    : PickupEntity("ShieldPickup", ParseColorHexCodeSRGB(0x49BCE3)) { }
	
	void ShieldPickupEntity::OnPickedUp(PlayerEntity& playerEntity)
	{
		playerEntity.SpawnShield(50);
		pickupEffectPlayer.Play(GetTransform().GetPosition(), 0.5f, 1.0f);
	}
	
	const char* ShieldPickupEntity::GetObjectName() const
	{
		return "Shield Pickup";
	}
	
	void ShieldPickupEntity::RenderProperties()
	{
		RenderTransformProperty(Transform::Properties::Position);
	}
	
	const char* ShieldPickupEntity::GetSerializeClassName() const
	{
		return "ShieldPickup";
	}
	
	nlohmann::json ShieldPickupEntity::Serialize() const
	{
		nlohmann::json json;
		
		json["transform"] = GetTransform().Serialize(Transform::Properties::Position);
		
		return json;
	}
	
	std::unique_ptr<Entity> ShieldPickupEntity::Clone() const
	{
		return std::make_unique<ShieldPickupEntity>();
	}
}
