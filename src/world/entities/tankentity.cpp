#include "tankentity.h"
#include "projectiles/plasmabulletentity.h"
#include "projectiles/rocketentity.h"
#include "shieldentity.h"
#include "../gameworld.h"
#include "../../settings.h"
#include "../../audio/soundsmanager.h"
#include "../../graphics/spriterenderlist.h"
#include "../../updateinfo.h"
#include "../../utils/random.h"
#include "../../utils/mathutils.h"
#include "../../lua/luavm.h"

#include <imgui.h>

namespace TankGame
{
	static const float SIZE = 0.35f;
	static const float CANNON_SIZE = 0.15f;
	
	TankEntity::TankEntity(glm::vec3 spotlightColor, const TankEntity::TextureInfo& textureInfo, int teamID, float maxHp)
	    : SpotLightEntity(spotlightColor, 5, glm::radians(80.0f), Attenuation(0, 0.7f)), Hittable(maxHp, teamID),
	      m_audioSource(AudioSource::VolumeModes::Effect), m_textureInfo(textureInfo), m_teamID(teamID)
	{
		SetFlickers(true);
		
		float baseTextureAR = textureInfo.m_baseTextureHeight / static_cast<float>(textureInfo.m_baseTextureWidth);
		GetTransform().SetScale({ SIZE, SIZE * baseTextureAR });
		
		m_cannonTransform = GetBaseCannonTransform(textureInfo);
		
		if (Settings::instance.GetLightingQuality() != QualitySettings::Low)
			SetShadowMode(EntityShadowModes::Dynamic);
		else
			SetShadowMode(EntityShadowModes::None);
	}
	
	void TankEntity::OnKilled()
	{
		if (m_shieldHandle.IsAlive())
			m_shieldHandle->Despawn();
	}
	
	Transform TankEntity::GetBaseCannonTransform(const TankEntity::TextureInfo& textureInfo)
	{
		Transform transform;
		
		float cannonAR = textureInfo.m_cannonTextureWidth / static_cast<float>(textureInfo.m_cannonTextureHeight);
		transform.SetScale({ CANNON_SIZE, CANNON_SIZE / cannonAR });
		
		transform.SetCenterOfRotation({ 0.0f, 1.0f - cannonAR });
		
		return transform;
	}
	
	void TankEntity::Draw(SpriteRenderList& spriteRenderList) const
	{
		int frame = static_cast<int>(m_frame - m_textureInfo.m_baseTextureFrames *
		                             std::floor(m_frame / static_cast<float>(m_textureInfo.m_baseTextureFrames)));
		
		glm::vec2 forward = GetTransform().GetForward();
		glm::vec2 cannonForward = m_cannonTransform.GetForward();
		
		m_cannonTransform.SetPosition(GetTransform().GetPosition() + forward * m_textureInfo.m_cannonYOffset -
		                              cannonForward * m_cannonOffset);
		
		spriteRenderList.Add(GetTransform(), GetBaseMaterial(frame), 0.5f);
		spriteRenderList.Add(m_cannonTransform, GetCannonMaterial(), 0.4f);
	}
	
	void TankEntity::Update(const UpdateInfo& updateInfo)
	{
		m_cannonOffset -= m_cannonOffset * 15 * updateInfo.m_dt;
		m_cannonOffset = std::max(m_cannonOffset, 0.0f);
		
		m_audioSource.SetPosition(GetTransform().GetPosition());
		
		if (m_shieldHandle.IsAlive())
			m_shieldHandle->GetTransform().SetPosition(GetTransform().GetPosition());
	}
	
	void TankEntity::OnDespawning()
	{
		if (m_shieldHandle.IsAlive())
			m_shieldHandle->Despawn();
	}
	
	void TankEntity::Fire(std::unique_ptr<Entity>&& bullet, float gameTime, float rotationOffset)
	{
		bullet->GetTransform().SetPosition(m_cannonTransform.GetPosition());
		bullet->GetTransform().SetRotation(m_cannonTransform.GetRotation() + rotationOffset);
		
		GetGameWorld()->Spawn(std::move(bullet));
		
		m_lastFireTime = gameTime;
		m_cannonOffset = 0.05f;
	}
	
	static std::uniform_real_distribution<float> pitchDist(1.0f, 1.0f);
	
	void TankEntity::FirePlasmaGun(glm::vec3 bulletColor, float damage, float gameTime, const FireParameters& params)
	{
		auto projectile = std::make_unique<PlasmaBulletEntity>(bulletColor, m_teamID, this, damage);
		projectile->SetIsHoming(params.m_homing);
		
		Fire(std::move(projectile), gameTime, params.m_rotationOffset);
		
		m_audioSource.SetBuffer(SoundsManager::GetInstance().GetSound("PlasmaGun"));
		m_audioSource.SetPitch(pitchDist(globalRNG));
		m_audioSource.Play();
		
		m_fireCooldown = 0.25f;
	}
	
	void TankEntity::FireRocket(float damage, float gameTime, const FireParameters& params)
	{
		auto projectile = std::make_unique<RocketEntity>(GetGameWorld()->GetParticlesManager(), m_teamID, this, damage);
		projectile->SetIsHoming(params.m_homing);
		
		Fire(std::move(projectile), gameTime, params.m_rotationOffset);
		
		m_fireCooldown = 1.0f;
	}
	
	void TankEntity::SpawnShield(float hp)
	{
		if (GetGameWorld() == nullptr)
			Panic("SpawnShield called before OnSpawned.");
		if (m_shieldHandle.IsAlive())
			m_shieldHandle->Despawn();
		
		float shieldRadius = GetTransform().GetBoundingCircle().GetRadius() * 1.3f;
		m_shieldHandle = GetGameWorld()->Spawn(std::make_unique<ShieldEntity>(hp, GetTeamID(), shieldRadius));
	}
	
	nlohmann::json TankEntity::Serialize() const
	{
		return Entity::Serialize();
	}
	
	ColliderInfo TankEntity::GetColliderInfo() const
	{
		return GetTransform().GetBoundingCircle();
	}
	
	bool TankEntity::IsStaticCollider() const
	{
		return false;
	}
	
	void TankEntity::RenderProperties()
	{
		RenderBaseProperties(Transform::Properties::Position | Transform::Properties::Rotation);
		
		float hp = GetHp();
		if (ImGui::SliderFloat("Hp", &hp, 1, GetMaxHp()))
			Hittable::SetHp(glm::clamp(hp, 1.0f, GetMaxHp()));
	}
	
	bool TankEntity::CanFire(float gameTime) const
	{
		return gameTime > m_lastFireTime + m_fireCooldown;
	}
}
