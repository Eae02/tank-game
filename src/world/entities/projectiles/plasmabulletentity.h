#pragma once

#include "projectileentity.h"
#include "../../lights/pointlightentity.h"

namespace TankGame
{
	class PlasmaBulletEntity : public PointLightEntity, public ProjectileEntity
	{
	public:
		explicit PlasmaBulletEntity(glm::vec3 color, int teamID, float damage);
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual const char* GetSerializeClassName() const override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
	protected:
		virtual void OnImpact() override;
		
		virtual void OnDeflected(glm::vec2 deflectionPos) override;
		
		virtual bool ShouldDeflect(const class DeflectionFieldEntity& deflectionField) const;
		
	private:
		bool m_isFading = false;
	};
}
