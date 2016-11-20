#include "hppickupentity.h"
#include "../playerentity.h"
#include "../../gameworld.h"
#include "../../../utils/utils.h"
#include "../../../audio/soundeffectplayer.h"

namespace TankGame
{
	static SoundEffectPlayer soundEffectPlayer{ "HpPickup" };
	
	HpPickupEntity::HpPickupEntity(float healAmount)
	    : PickupEntity("HpPickup", ParseColorHexCodeSRGB(0xE40500)), m_healAmount(healAmount)
	{
		GetTransform().SetScale(glm::vec2(0.2f));
	}
	
	void HpPickupEntity::OnPickedUp(PlayerEntity& playerEntity)
	{
		soundEffectPlayer.Play(GetTransform().GetPosition(), 0.5f, 1.0f);
		playerEntity.SetHp(playerEntity.GetHp() + m_healAmount);
	}
	
	static std::uniform_int_distribution<int> amountDist(2, 4);
	static std::uniform_real_distribution<float> angleDist(0.0f, glm::two_pi<float>());
	static std::uniform_real_distribution<float> magnitudeDist(2.0f, 4.0f);
	static std::uniform_real_distribution<float> healAmountDist(0.7f, 1.3f);
	
	void HpPickupEntity::SpawnEntities(GameWorld& gameWorld, glm::vec2 position, float totalHeal)
	{
		int numberToSpawn = amountDist(randomGen);
		float avgHealPerEntity = totalHeal / numberToSpawn;
		
		for (int i = 0; i < numberToSpawn; i++)
		{
			float healAmount = avgHealPerEntity * healAmountDist(randomGen);
			std::unique_ptr<HpPickupEntity> entity = std::make_unique<HpPickupEntity>(healAmount);
			
			float angle = angleDist(randomGen);
			float mag = magnitudeDist(randomGen);
			
			entity->GetTransform().SetPosition(position);
			entity->SetVelocity({ mag * std::cos(angle), mag * std::sin(angle) });
			
			gameWorld.Spawn(std::move(entity));
		}
	}
}
