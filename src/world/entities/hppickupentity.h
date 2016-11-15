#pragma once

#include "pickupentity.h"

namespace TankGame
{
	class HpPickupEntity : public PickupEntity
	{
	public:
		explicit HpPickupEntity(float healAmount);
		
		inline float GetHealAmount() const
		{ return m_healAmount; }
		
		static void SpawnEntities(class GameWorld& gameWorld, glm::vec2 position, float totalHeal);
		
	protected:
		virtual void OnPickedUp(class PlayerEntity& playerEntity) override;
		
	private:
		float m_healAmount;
	};
}
