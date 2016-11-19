#include "plasmabulletentity.h"
#include "../particlesystementity.h"
#include "../hittable.h"
#include "../deflectionfieldentity.h"
#include "../../spteams.h"
#include "../../gameworld.h"
#include "../../particles/systems/sparkparticlesystem.h"
#include "../../particles/systems/blueorbparticlesystem.h"
#include "../../../audio/soundeffectplayer.h"
#include "../../../updateinfo.h"
#include "../../../utils/utils.h"

#include <random>

namespace TankGame
{
	static SoundEffectPlayer impactEffectPlayer{ "BulletImpact" };
	
	PlasmaBulletEntity::PlasmaBulletEntity(glm::vec3 color, int teamID, const Entity* sourceEntity, float damage)
	    : PointLightEntity(color, 4, Attenuation(0, 1.5f), 0.1f),
	      ProjectileEntity(teamID, sourceEntity, 18, damage, 0.5f)
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
	
	void PlasmaBulletEntity::OnDeflected(glm::vec2 deflectionPos)
	{
		BlueOrbParticleSystem particleSystem(GetGameWorld()->GetParticlesManager());
		
		auto entity = std::make_unique<ParticleSystemEntity<BlueOrbParticleSystem>>(std::move(particleSystem), 0.05f);
		
		entity->GetTransform().SetPosition(deflectionPos);
		
		GetGameWorld()->Spawn(std::move(entity));
		
		impactEffectPlayer.Play(deflectionPos, 0.5f, 2.0f);
	}
	
	bool PlasmaBulletEntity::ShouldDeflect(const DeflectionFieldEntity& deflectionField) const
	{
		if (!deflectionField.DeflectEnemy() && GetTeamID() == EnemyTeamID)
			return false;
		return true;
	}
	
	const char* PlasmaBulletEntity::GetSerializeClassName() const
	{
		return nullptr;
	}
	
	std::unique_ptr<Entity> PlasmaBulletEntity::Clone() const
	{
		auto clone = std::make_unique<PlasmaBulletEntity>(GetColor(), GetTeamID(), this, GetDamage());
		clone->GetTransform() = GetTransform();
		return clone;
	}
}
