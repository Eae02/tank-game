#pragma once

#include "pickupentity.h"

namespace TankGame
{
	class ShieldPickupEntity : public PickupEntity
	{
	public:
		ShieldPickupEntity();
		
		virtual const char* GetObjectName() const override;
		virtual void RenderProperties() override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		
	protected:
		virtual void OnPickedUp(class PlayerEntity& playerEntity) override;
	};
}
