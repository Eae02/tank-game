#include "pickupentity.h"
#include "../playerentity.h"
#include "../../icollidable.h"
#include "../../gameworld.h"
#include "../../../utils/mathutils.h"
#include "../../../utils/utils.h"
#include "../../../updateinfo.h"
#include "../../../graphics/spriterenderlist.h"

#include <glm/gtc/constants.hpp>
#include <random>

namespace TankGame
{
	static const float INTENSITY = 9;
	static const float INTENSITY_VARIANCE = 3;
	static const float PULSE_SPEED = 4;
	
	static std::uniform_real_distribution<float> rotationDist(0, glm::two_pi<float>());
	
	PickupEntity::PickupEntity(const std::string& propClassName, glm::vec3 glowColor)
	    : PointLightEntity(glowColor, INTENSITY, Attenuation(0, 2), 0.1f), PropEntity(propClassName)
	{
		GetTransform().SetScale(glm::vec2(0.25f));
		GetTransform().SetRotation(rotationDist(randomGen));
		SetZ(0.7f);
	}
	
	void PickupEntity::Update(const UpdateInfo& updateInfo)
	{
		SetIntensity(INTENSITY + std::sin(updateInfo.m_gameTime * PULSE_SPEED) * INTENSITY_VARIANCE);
		
		glm::vec2 oldVelocity = m_velocity;
		
		m_velocity -= m_velocity * glm::min(updateInfo.m_dt * 5, 1.0f);
		
		if (m_playerEntity != nullptr)
		{
			glm::vec2 toPlayer = m_playerEntity->GetTransform().GetPosition() - GetTransform().GetPosition();
			float distToPlayerSq = LengthSquared(toPlayer);
			
			if (distToPlayerSq < 0.2f)
			{
				OnPickedUp(*m_playerEntity);
				Despawn();
				return;
			}
			else if (distToPlayerSq < 3)
			{
				toPlayer = glm::normalize(toPlayer);
				
				m_velocity += toPlayer * glm::min(updateInfo.m_dt * 15, 1.0f);
			}
		}
		
		GetTransform().Translate((m_velocity + oldVelocity) * 0.5f * updateInfo.m_dt);
		
		if (glm::dot(m_velocity, m_velocity) > 1E-6)
		{
			IntersectInfo intersectInfo = GetGameWorld()->GetIntersectInfo(GetTransform().GetBoundingCircle(),
			                                                               [] (const ICollidable& collidable)
			{
				//The pickup should only collide with object collidables
				return collidable.GetCollidableType() == CollidableTypes::Object;
			});
			
			if (intersectInfo.m_intersects)
				GetTransform().Translate(-intersectInfo.m_penetration);
		}
	}
	
	void PickupEntity::OnSpawned(GameWorld& gameWorld)
	{
		m_playerEntity = dynamic_cast<PlayerEntity*>(gameWorld.GetEntityByName("player"));
		
		PointLightEntity::OnSpawned(gameWorld);
	}
	
	void PickupEntity::RenderProperties()
	{
		Entity::RenderProperties();
	}
	
	const char* PickupEntity::GetObjectName() const
	{
		return "Pickup";
	}
	
	std::unique_ptr<Entity> PickupEntity::Clone() const
	{
		return nullptr;
	}
}
