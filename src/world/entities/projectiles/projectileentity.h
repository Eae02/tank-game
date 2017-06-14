#pragma once

#include "../../entity.h"
#include "../../entityhandle.h"

namespace TankGame
{
	class ProjectileEntity : public virtual Entity, public Entity::IUpdateable
	{
	public:
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		
		inline int GetTeamID() const
		{ return m_teamID; }
		inline float GetDamage() const
		{ return m_damage; }
		
		inline void SetIsHoming(bool isHoming)
		{ m_isHoming = isHoming; }
		inline bool IsHoming() const
		{ return m_isHoming; }
		
	protected:
		enum ImpactFlags
		{
			DefaultImpact = 0,
			EntityImpact = 1,
			ShieldImpact = 2
		};
		
		ProjectileEntity(int teamID, const Entity* sourceEntity, float speed, float damage, float damageStandardDist);
		
		virtual void OnImpact(ImpactFlags flags, glm::vec2 penetration) { }
		
		virtual bool ShouldDeflect(const class DeflectionFieldEntity& deflectionField) const;
		virtual void OnDeflected(glm::vec2 deflectionPos) { }
		
	private:
		bool SearchForHomeTarget();
		
		bool m_isHoming = false;
		EntityHandle m_homeTarget;
		
		int m_teamID;
		const Entity* m_sourceEntity;
		
		float m_speed;
		float m_damage;
		
		bool m_hasDeflected = false;
	};
}
