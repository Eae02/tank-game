#include "projectileentity.h"
#include "../particlesystementity.h"
#include "../hittable.h"
#include "../deflectionfieldentity.h"
#include "../../gameworld.h"
#include "../../../updateinfo.h"
#include "../../../utils/utils.h"

#include <random>
#include <glm/gtc/constants.hpp>

namespace TankGame
{
	ProjectileEntity::ProjectileEntity(int teamID, const Entity* sourceEntity, float speed, float damage, float damageStandardDist)
	    : m_teamID(teamID), m_sourceEntity(sourceEntity), m_speed(speed),
	      m_damage(std::normal_distribution<float>(damage, damageStandardDist)(randomGen)) { }
	
	bool ProjectileEntity::ShouldDeflect(const class DeflectionFieldEntity& deflectionField) const
	{
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
		
		const float CIRCLE_RADIUS = 0.1f;
		Circle circle(GetTransform().GetPosition(), CIRCLE_RADIUS);
		
		//Checks for intersections with tiles and object type entities
		IntersectInfo intersectInfo = GetGameWorld()->GetIntersectInfo(circle, [] (const Entity& entity)
		{
			return entity.GetSolidType() == SolidTypes::Object;
		});
		
		if (intersectInfo.m_intersects)
		{
			GetTransform().Translate(-intersectInfo.m_penetration + move * CIRCLE_RADIUS);
			OnImpact();
		}
		
		//Checks for intersections with hittable entities
		GetGameWorld()->IterateIntersectingEntities(circle.GetBoundingRectangle(), [&] (Entity& entity)
		{
			if (&entity == m_sourceEntity)
				return;
			
			Hittable* hittable = entity.AsHittable();
			
			if (hittable != nullptr && (hittable->GetTeamID() != m_teamID || m_hasDeflected) &&
			    circle.Intersects(hittable->GetHitCircle()))
			{
				hittable->SetHp(hittable->GetHp() - m_damage);
				OnImpact();
			}
		});
	}
}
