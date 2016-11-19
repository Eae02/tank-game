#pragma once

#include "../../entity.h"

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
		
	protected:
		ProjectileEntity(int teamID, const Entity* sourceEntity, float speed, float damage, float damageStandardDist);
		
		virtual void OnImpact() { }
		
		virtual bool ShouldDeflect(const class DeflectionFieldEntity& deflectionField) const;
		virtual void OnDeflected(glm::vec2 deflectionPos) { }
		
	private:
		int m_teamID;
		const Entity* m_sourceEntity;
		
		float m_speed;
		float m_damage;
		
		bool m_hasDeflected = false;
	};
}
