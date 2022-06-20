#include "energyball.h"
#include "explosionentity.h"
#include "../spteams.h"
#include "../icollidable.h"
#include "../gameworld.h"
#include "../particles/systems/sparkparticlesystem.h"

#include "../../updateinfo.h"
#include "../../graphics/spriterenderlist.h"
#include "../../utils/ioutils.h"
#include "../../utils/random.h"
#include "../../utils/mathutils.h"

#include <glm/gtc/constants.hpp>
#include <imgui.h>

namespace TankGame
{
	static std::uniform_real_distribution<float> damageMulDist(1.0f, 1.2f);
	
	EnergyBall::EnergyBall(glm::vec2 direction, float damage, ParticlesManager& particlesManager)
	    : PointLightEntity(ParseColorHexCodeSRGB(0xFFE736), 3, Attenuation(0, 1)),
	      ParticleSystemEntity(EnergyBallParticleSystem(particlesManager)),
	      Hittable(20, NeutralTeamID), m_damage(damage), m_direction(glm::normalize(direction)),
	      m_directionAngle(std::atan2(m_direction.y, m_direction.x))
	{
		SetShadowMode(EntityShadowModes::Dynamic);
	}
	
	void EnergyBall::Update(const UpdateInfo& updateInfo)
	{
		GetTransform().Translate(m_direction * updateInfo.m_dt * 1.5f);
		
		Circle circle = GetHitCircle();
		
		//Checks for intersections with tiles and object type collidable entities
		IntersectInfo intersectInfo = GetGameWorld()->GetIntersectInfo(circle, [] (const ICollidable& collidable)
		{
			return collidable.GetCollidableType() == CollidableTypes::Object;
		});
		
		//Bounces the energy ball off tiles and object type collidable entities
		if (intersectInfo.m_intersects)
		{
			m_direction = glm::reflect(m_direction, glm::normalize(intersectInfo.m_penetration));
			
			GetTransform().Translate(-m_direction * glm::dot(intersectInfo.m_penetration, m_direction));
			
			//Spawns a spark particle system
			SparkParticleSystem particleSystem(GetGameWorld()->GetParticlesManager());
			
			auto psEntity = std::make_unique<ParticleSystemEntity<SparkParticleSystem>>(std::move(particleSystem), 0.05f);
			
			psEntity->GetTransform().SetRotation(std::atan2(intersectInfo.m_penetration.y, intersectInfo.m_penetration.x) + glm::pi<float>());
			psEntity->GetTransform().SetPosition(GetTransform().GetPosition());
			
			GetGameWorld()->Spawn(std::move(psEntity));
		}
		
		Hittable* hitEntity = nullptr;
		glm::vec2 hitEntityPenetration;
		float hitEntityPenetrationLenSquared = 0;
		
		//Checks for intersections with hittable entities
		GetGameWorld()->IterateIntersectingEntities(circle.GetBoundingRectangle(), [&] (Entity& entity)
		{
			if (&entity == this)
				return;
			
			Hittable* hittable = entity.AsHittable();
			if (hittable == nullptr)
				return;
			
			IntersectInfo intersectInfo = circle.GetIntersectInfo(hittable->GetHitCircle());
			if (!intersectInfo.m_intersects)
				return;
			
			float penetrationLenSquared = LengthSquared(intersectInfo.m_penetration);
			
			if (penetrationLenSquared > hitEntityPenetrationLenSquared)
			{
				hitEntity = hittable;
				hitEntityPenetrationLenSquared = penetrationLenSquared;
				hitEntityPenetration = intersectInfo.m_penetration;
			}
		});
		
		if (hitEntity != nullptr)
		{
			//Spawns an explosion at the energy ball's position.
			auto explosionEntity = std::make_unique<ExplosionEntity>(GetGameWorld()->GetParticlesManager());
			explosionEntity->GetTransform().SetPosition(GetTransform().GetPosition());
			GetGameWorld()->Spawn(std::move(explosionEntity));
			
			hitEntity->SetHp(hitEntity->GetHp() - m_damage * damageMulDist(globalRNG));
			Despawn();
		}
	}
	
	Circle EnergyBall::GetHitCircle() const
	{
		return Circle(GetTransform().GetPosition(), 0.2f);
	}
	
	void EnergyBall::DrawDistortions() const
	{
		
	}
	
	void EnergyBall::OnSpawned(GameWorld& gameWorld)
	{
		LightSourceEntity::OnSpawned(gameWorld);
		ParticleSystemEntity::OnSpawned(gameWorld);
	}
	
	const char* EnergyBall::GetSerializeClassName() const
	{
		return "EnergyBall";
	}
	
	nlohmann::json EnergyBall::Serialize() const
	{
		nlohmann::json json;
		
		json["transform"] = GetTransform().Serialize(Transform::Properties::Position);
		json["direction"] =  { m_direction.x, m_direction.y };
		
		return json;
	}
	
	std::unique_ptr<Entity> EnergyBall::Clone() const
	{
		auto clone = std::make_unique<EnergyBall>(m_direction, m_damage, GetGameWorld()->GetParticlesManager());
		clone->GetTransform() = GetTransform();
		return clone;
	}
	
	void EnergyBall::RenderProperties()
	{
		RenderBaseProperties(Transform::Properties::Position);
		
		if (ImGui::SliderAngle("Rotation", &m_directionAngle))
		{
			m_direction = { std::cos(m_directionAngle), std::sin(m_directionAngle) };
		}
		
		if (ImGui::SliderFloat("Damage", &m_damage, 0.0f, 100.0f))
			m_damage = glm::clamp(m_damage, 0.0f, 100.0f);
	}
	
	const char* EnergyBall::GetObjectName() const
	{
		return "Energy Ball";
	}
}
