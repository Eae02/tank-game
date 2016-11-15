#include "projectileentity.h"
#include "../particlesystementity.h"
#include "../hittable.h"
#include "../../gameworld.h"
#include "../../../updateinfo.h"
#include "../../../utils/utils.h"

#include <random>

namespace TankGame
{
	ProjectileEntity::ProjectileEntity(int teamID, float speed, float damage, float damageStandardDist)
	    : m_teamID(teamID), m_speed(speed),
	      m_damage(std::normal_distribution<float>(damage, damageStandardDist)(randomGen))
	{
		
	}
	
	void ProjectileEntity::Update(const UpdateInfo& updateInfo)
	{
		glm::vec2 move = GetTransform().GetForward();
		GetTransform().Translate(move * updateInfo.m_dt * m_speed);
		
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
			Hittable* hittable = entity.AsHittable();
			
			if (hittable != nullptr && hittable->GetTeamID() != m_teamID && circle.Intersects(hittable->GetHitCircle()))
			{
				hittable->SetHp(hittable->GetHp() - m_damage);
				OnImpact();
			}
		});
	}
}
