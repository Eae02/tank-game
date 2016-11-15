#include "plasmabulletentity.h"
#include "../particlesystementity.h"
#include "../hittable.h"
#include "../../gameworld.h"
#include "../../particles/systems/sparkparticlesystem.h"
#include "../../../audio/soundeffectplayer.h"
#include "../../../updateinfo.h"
#include "../../../utils/utils.h"

#include <random>

namespace TankGame
{
	static SoundEffectPlayer impactEffectPlayer{ "BulletImpact" };
	
	PlasmaBulletEntity::PlasmaBulletEntity(glm::vec3 color, int teamID, float damage)
	    : PointLightEntity(color, 4, Attenuation(0, 1.5f), 0.1f),
	      ProjectileEntity(teamID, 18, damage, 0.5f)
	{
		
	}
	
	void PlasmaBulletEntity::Update(const UpdateInfo& updateInfo)
	{
		if (m_isFading)
		{
			const float FADE_SPEED = 50;
			
			float newIntensity = GetIntensity() - updateInfo.m_dt * FADE_SPEED;
			if (newIntensity < 0)
				Despawn();
			SetIntensity(newIntensity);
		}
		else
		{
			ProjectileEntity::Update(updateInfo);
		}
	}
	
	void PlasmaBulletEntity::OnImpact()
	{
		impactEffectPlayer.Play(GetTransform().GetPosition(), 0.7f, 1.0f);
		
		m_isFading = true;
		SetColor(ParseColorHexCodeSRGB(0xF5CB42));
		SetIntensity(10);
		
		SparkParticleSystem particleSystem(GetGameWorld()->GetParticlesManager());
		
		auto entity = std::make_unique<ParticleSystemEntity<SparkParticleSystem>>(std::move(particleSystem), 0.05f);
		
		entity->GetTransform().SetRotation(GetTransform().GetRotation() + glm::radians(90.0f));
		entity->GetTransform().SetPosition(GetTransform().GetPosition());
		
		GetGameWorld()->Spawn(std::move(entity));
	}
	
	const char* PlasmaBulletEntity::GetSerializeClassName() const
	{
		return nullptr;
	}
	
	std::unique_ptr<Entity> PlasmaBulletEntity::Clone() const
	{
		auto clone = std::make_unique<PlasmaBulletEntity>(GetColor(), GetTeamID(), GetDamage());
		clone->GetTransform() = GetTransform();
		return clone;
	}
}
