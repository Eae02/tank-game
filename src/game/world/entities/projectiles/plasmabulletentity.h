#pragma once

#include "projectileentity.h"
#include "../../lights/pointlightentity.h"

namespace TankGame
{
	class PlasmaBulletEntity : public PointLightEntity, public ProjectileEntity
	{
	public:
		explicit PlasmaBulletEntity(glm::vec3 color, int teamID, const Entity* sourceEntity, float damage);
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual const char* GetSerializeClassName() const override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
	protected:
		virtual void OnImpact(ImpactFlags flags, glm::vec2 penetration) override;
		
		virtual void OnDeflected(glm::vec2 deflectionPos) override;
		
		virtual bool ShouldDeflect(const class DeflectionFieldEntity& deflectionField) const;
		
	private:
		bool m_isFading = false;
	};
}
