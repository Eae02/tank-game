#include "projectileentity.h"
#include "../particlesystementity.h"
#include "../hittable.h"
#include "../deflectionfieldentity.h"
#include "../shieldentity.h"
#include "../../gameworld.h"
#include "../../../updateinfo.h"
#include "../../../utils/mathutils.h"
#include "../../../utils/random.h"

#include <random>
#include <glm/gtc/constants.hpp>

namespace TankGame
{
	ProjectileEntity::ProjectileEntity(int teamID, const Entity* sourceEntity, float speed, float damage, float damageStandardDist)
	    : m_teamID(teamID), m_sourceEntity(sourceEntity), m_speed(speed),
	      m_damage(std::normal_distribution<float>(damage, damageStandardDist)(globalRNG)) { }
	
	bool ProjectileEntity::ShouldDeflect(const class DeflectionFieldEntity& deflectionField) const
	{
		return false;
	}
	
	bool ProjectileEntity::SearchForHomeTarget()
	{
		const float TARGET_MAX_DISTANCE = 10;
		Rectangle targetSearchArea = Rectangle::CreateCentered(GetTransform().GetPosition() +
		                                                       GetTransform().GetForward() * TARGET_MAX_DISTANCE * 0.5f,
		                                                       TARGET_MAX_DISTANCE, TARGET_MAX_DISTANCE);
		
		Entity* homeTarget = nullptr;
		float distToHomeTargetSq;
		
		GetGameWorld()->IterateIntersectingEntities(targetSearchArea, [&] (Entity& entity)
		{
			const Hittable* hittable = entity.AsHittable();
			if (hittable == nullptr || hittable->GetTeamID() == m_teamID || &entity == m_sourceEntity)
				return;
			
			float distToTargetSq = LengthSquared(entity.GetTransform().GetPosition() - GetTransform().GetPosition());
			
			if (homeTarget == nullptr || distToHomeTargetSq < distToTargetSq)
			{
				//Checks that there are no walls between the target and the projectile.
				if (!GetGameWorld()->IsRayObstructed(GetTransform().GetPosition(), entity.GetTransform().GetPosition(),
				                                     ICollidable::IsObject))
				{
					homeTarget = &entity;
					distToHomeTargetSq = distToTargetSq;
				}
			}
		});
		
		if (homeTarget != nullptr)
		{
			m_homeTarget = { *GetGameWorld(), *homeTarget };
			return true;
		}
		
		return false;
	}
	
	void ProjectileEntity::Update(const UpdateInfo& updateInfo)
	{
		glm::vec2 oldPos = GetTransform().GetPosition();
		
		glm::vec2 move = GetTransform().GetForward();
		GetTransform().Translate(move * updateInfo.m_dt * m_speed);
		
		//The rectangle to check for deflection fields in
		Rectangle deflectionFieldRect = Rectangle::FromMinMax(oldPos, GetTransform().GetPosition());
		
		//Checks for intersections with deflection fields
		bool deflected = false;
		GetGameWorld()->IterateIntersectingEntities(deflectionFieldRect, [&] (Entity& entity)
		{
			if (deflected)
				return;
			
			DeflectionFieldEntity* dfEntity = dynamic_cast<DeflectionFieldEntity*>(&entity);
			if (dfEntity == nullptr || !ShouldDeflect(*dfEntity))
				return;
			
			glm::vec2 fieldForward = dfEntity->GetTransform().GetForward();
			glm::vec2 fieldNormal(fieldForward.y, -fieldForward.x);
			glm::vec2 fieldStart = dfEntity->GetTransform().GetPosition();
			
			float fieldOffset = glm::dot(fieldNormal, dfEntity->GetTransform().GetPosition());
			
			//Checks that the projectile crossed the field this frame
			if (std::signbit(glm::dot(fieldNormal, oldPos) - fieldOffset) ==
			    std::signbit(glm::dot(fieldNormal, GetTransform().GetPosition()) - fieldOffset))
			{
				return;
			}
			
			//Checks that the deflection point is on the field
			float posAlongField = (oldPos.y * move.x - oldPos.x * move.y + fieldStart.x * move.y - fieldStart.y * move.x)
			        / (fieldForward.y * move.x - fieldForward.x * move.y);
			if (posAlongField < 0.0f || posAlongField > dfEntity->GetLength())
				return;
			
			OnDeflected(fieldStart + posAlongField * fieldForward);
			dfEntity->Flash();
			
			deflected = true;
			
			glm::vec2 newMove = glm::reflect(move, fieldNormal);
			
			GetTransform().Translate(newMove * updateInfo.m_dt * m_speed);
			GetTransform().SetRotation(glm::half_pi<float>() + std::atan2(newMove.y, newMove.x));
		});
		
		if (deflected)
		{
			m_hasDeflected = true;
			m_damage *= 0.5f;
			return;
		}
		
		if (m_isHoming && (m_homeTarget.IsAlive() || SearchForHomeTarget()))
		{
			const float HOME_FACTOR = 20;
			
			glm::vec2 toTarget = m_homeTarget->GetTransform().GetPosition() - GetTransform().GetPosition();
			float targetRotation = glm::half_pi<float>() + std::atan2(toTarget.y, toTarget.x);
			
			float rotation = GetTransform().GetRotation();
			UpdateTransition(rotation, targetRotation, HOME_FACTOR * updateInfo.m_dt / glm::length(toTarget));
			GetTransform().SetRotation(rotation);
		}
		
		const float CIRCLE_RADIUS = 0.1f;
		Circle circle(GetTransform().GetPosition(), CIRCLE_RADIUS);
		
		//Checks for intersections with tiles and object type collidable entities
		IntersectInfo intersectInfo = GetGameWorld()->GetIntersectInfo(circle, [] (const ICollidable& collidable)
		{
			return collidable.GetCollidableType() == CollidableTypes::Object;
		});
		
		if (intersectInfo.m_intersects)
		{
			GetTransform().Translate(-intersectInfo.m_penetration + move * CIRCLE_RADIUS);
			OnImpact(DefaultImpact, intersectInfo.m_penetration);
		}
		
		Hittable* hitEntity = nullptr;
		glm::vec2 hitEntityPenetration;
		float hitEntityPenetrationLenSquared = 0;
		
		//Checks for intersections with hittable entities
		GetGameWorld()->IterateIntersectingEntities(circle.GetBoundingRectangle(), [&] (Entity& entity)
		{
			if (&entity == m_sourceEntity && !m_hasDeflected)
				return;
			
			Hittable* hittable = entity.AsHittable();
			if (hittable == nullptr || (hittable->GetTeamID() == m_teamID && !m_hasDeflected))
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
			hitEntity->SetHp(hitEntity->GetHp() - m_damage);
			GetTransform().Translate(-hitEntityPenetration);
			
			ImpactFlags flags = EntityImpact;
			
			ShieldEntity* shieldEntity = dynamic_cast<ShieldEntity*>(hitEntity);
			if (shieldEntity != nullptr)
			{
				glm::vec2 toCenterShield = shieldEntity->GetTransform().GetPosition() - GetTransform().GetPosition();
				shieldEntity->Ripple(std::atan2(-toCenterShield.y, -toCenterShield.x));
				
				flags = static_cast<ImpactFlags>(flags | ShieldImpact);
			}
			
			OnImpact(flags, hitEntityPenetration);
		}
	}
}
