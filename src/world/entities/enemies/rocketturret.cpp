#include "rocketturret.h"
#include "enemytank.h"
#include "../shieldentity.h"
#include "../explosionentity.h"
#include "../playerentity.h"
#include "../projectiles/rocketentity.h"
#include "../../props/propsmanager.h"
#include "../../spteams.h"
#include "../../gameworld.h"
#include "../../../utils/random.h"
#include "../../../utils/mathutils.h"
#include "../../../utils/ioutils.h"
#include "../../../updateinfo.h"
#include "../../../graphics/spriterenderlist.h"
#include "../../../audio/soundsmanager.h"
#include "../../../graphics/ui/uirenderer.h"

#include <imgui.h>
#include <glm/glm.hpp>

namespace TankGame
{
	static const glm::vec3 SCAN_COLOR = ParseColorHexCodeSRGB(0xFF0000);
	static const glm::vec3 SCAN_BLINK_COLOR = ParseColorHexCodeSRGB(0xFFD52E);
	
	RocketTurret::RocketTurret(float minRotationAngle, float maxRotationAngle, float rotationSpeed, float fireDelay)
	    : RayLightEntity(SCAN_COLOR, 10, Attenuation(0, 10), 10, 0.05f),
	      Hittable(100, EnemyTeamID), m_minRotationAngle(minRotationAngle), m_maxRotationAngle(maxRotationAngle),
	      m_rotationSpeed(rotationSpeed), m_fireDelay(fireDelay),
	      m_cannonPropClass(*PropsManager::GetInstance().GetPropClassByName("TurretCannon")),
	      m_ambienceSource(AudioSource::VolumeModes::Effect), m_detectedSource(AudioSource::VolumeModes::Effect)
	{
		const float CANNON_SIZE = 0.3f;
		
		SetFlickerIntensity(0.65f);
		
		float cannonAR = m_cannonPropClass.GetTextureWidth() / static_cast<float>(m_cannonPropClass.GetTextureHeight());
		
		GetTransform().SetScale({ CANNON_SIZE, CANNON_SIZE / cannonAR });
		GetTransform().SetCenterOfRotation({ 0.0f, 1.0f - cannonAR });
		
		m_ambienceSource.SetVolume(0.75f);
		m_detectedSource.SetAttenuationSettings(1.0f, 5.0f);
		m_ambienceSource.SetIsLooping(true);
		m_ambienceSource.SetBuffer(SoundsManager::GetInstance().GetSound("Beam"));
		
		m_detectedSource.SetVolume(2.0f);
		m_detectedSource.SetAttenuationSettings(0.5f, 1.0f);
		m_detectedSource.SetBuffer(SoundsManager::GetInstance().GetSound("TurretDetected"));
	}
	
	void RocketTurret::Draw(SpriteRenderList& spriteRenderList) const
	{
		spriteRenderList.Add(GetTransform(), m_cannonPropClass.GetMaterial(), 0.35f);
	}
	
	void RocketTurret::DrawEditorUI(UIRenderer& uiRenderer, const glm::mat3& viewTransform) const
	{
		float rotations[] = { m_baseRotation + m_minRotationAngle, m_baseRotation + m_maxRotationAngle };
		
		for (float rot : rotations)
		{
			Transform transform;
			transform.SetPosition(GetTransform().GetPosition());
			transform.SetRotation(rot);
			
			glm::vec2 forward = transform.GetForward();
			
			float distToWall = CalcLength(&forward);
			
			transform.Translate(forward * distToWall * 0.5f);
			transform.SetScale({ 0.01f, distToWall * 0.5f });
			
			uiRenderer.DrawQuad(viewTransform * transform.GetMatrix(), glm::vec4(1, 0, 0, 0.8f));
		}
	}
	
	static std::uniform_real_distribution<float> rocketDamageDist(40, 55);
	
	void RocketTurret::Update(const UpdateInfo& updateInfo)
	{
		const float FIRE_PREPERATION_TIME = 0.4f;
		const int PREP_BLINK_TIMES = 2;
		
		if (m_firePreparationTime > 0 && m_playerEntity != nullptr)
		{
			RotateTowardsPlayer(updateInfo.m_dt);
			
			if (glm::fract((m_firePreparationTime / FIRE_PREPERATION_TIME) * PREP_BLINK_TIMES) < 0.5f)
				SetColor(SCAN_BLINK_COLOR);
			else
				SetColor(SCAN_COLOR);
			
			m_firePreparationTime -= updateInfo.m_dt;
			
			if (m_firePreparationTime < 0)
			{
				m_firePreparationTime = 0;
				
				SetColor(SCAN_COLOR);
				
				auto rocket = std::make_unique<RocketEntity>(GetGameWorld()->GetParticlesManager(), NeutralTeamID,
				                                             this, rocketDamageDist(globalRNG));
				
				rocket->GetTransform().SetPosition(GetTransform().GetPosition());
				rocket->GetTransform().SetRotation(GetTransform().GetRotation());
				
				GetGameWorld()->Spawn(std::move(rocket));
			}
		}
		else
		{
			if (ShouldFire())
			{
				if (updateInfo.m_gameTime > m_nextFireTime)
				{
					m_firePreparationTime = FIRE_PREPERATION_TIME;
					m_nextFireTime = updateInfo.m_gameTime + m_fireDelay;
					
					m_detectedSource.Play();
				}
				else
					RotateTowardsPlayer(updateInfo.m_dt);
			}
			else
			{
				float relRotation = GetTransform().GetRotation() - m_baseRotation;
				float newRotation = relRotation + (m_spinDirection ? 1 : -1) * updateInfo.m_dt * m_rotationSpeed * 0.4f;
				
				if (newRotation < m_minRotationAngle)
				{
					m_spinDirection = true;
					newRotation = m_minRotationAngle;
				}
				else if (newRotation > m_maxRotationAngle)
				{
					m_spinDirection = false;
					newRotation = m_maxRotationAngle;
				}
				
				GetTransform().SetRotation(m_baseRotation + newRotation);
			}
		}
		
		glm::vec2 cannonForward = GetTransform().GetForward();
		
		SetLength(CalcLength(&cannonForward));
		
		m_ambienceSource.SetPosition(GetTransform().GetPosition());
		m_ambienceSource.SetDirection(cannonForward);
		
		m_detectedSource.SetPosition(GetTransform().GetPosition());
		m_detectedSource.SetDirection(cannonForward);
	}
	
	bool RocketTurret::ShouldFire() const
	{
		if (m_playerEntity == nullptr)
			return false;
		
		glm::vec2 toPlayer = m_playerEntity->GetTransform().GetPosition() - GetTransform().GetPosition();
		
		if (LengthSquared(toPlayer) > GetLength() * GetLength())
			return false;
		
		glm::vec2 forward = GetTransform().GetForward();
		glm::vec2 left(forward.y, -forward.x);
		
		if (glm::dot(toPlayer, forward) < 0)
			return false;
		if (std::abs(glm::dot(toPlayer, left)) > m_playerEntity->GetHitCircle().GetRadius() * 0.6f)
			return false;
		
		return true;
	}
	
	void RocketTurret::RotateTowardsPlayer(float dt)
	{
		glm::vec2 toPlayer = m_playerEntity->GetTransform().GetPosition() - GetTransform().GetPosition();
		float targetRot = std::atan2(toPlayer.y, toPlayer.x) + glm::half_pi<float>();
		float rotDiff = GetRotationDifference(targetRot, GetTransform().GetRotation());
		
		GetTransform().Rotate(-rotDiff * glm::min(dt * 5, 1.0f));
	}
	
	float RocketTurret::CalcLength(const glm::vec2* forward) const
	{
		glm::vec2 f = forward == nullptr ? GetTransform().GetForward() : *forward;
		
		return GetGameWorld()->GetRayIntersectionDistance(GetTransform().GetPosition(), f, [] (const ICollidable& c)
		{
			return c.GetCollidableType() == CollidableTypes::Object;
		});
	}
	
	void RocketTurret::OnSpawned(GameWorld& gameWorld)
	{
		m_baseRotation = GetTransform().GetRotation();
		GetTransform().Rotate(std::uniform_real_distribution<float>(m_minRotationAngle, m_maxRotationAngle)(globalRNG));
		
		m_playerEntity = dynamic_cast<const PlayerEntity*>(gameWorld.GetEntityByName("player"));
		
		if (gameWorld.GetWorldType() == GameWorld::Types::Game)
			m_ambienceSource.Play();
		
		RayLightEntity::OnSpawned(gameWorld);
		
		SetLength(CalcLength(nullptr));
	}
	
	ColliderInfo RocketTurret::GetColliderInfo() const
	{
		return GetTransform().GetInscribedCircle();
	}
	
	CollidableTypes RocketTurret::GetCollidableType() const
	{
		return CollidableTypes::Npc;
	}
	
	Circle RocketTurret::GetHitCircle() const
	{
		return GetTransform().GetBoundingCircle();
	}
	
	void RocketTurret::EditorMoved()
	{
		SetLength(CalcLength(nullptr));
	}
	
	void RocketTurret::OnKilled()
	{
		auto explosion = std::make_unique<ExplosionEntity>(GetGameWorld()->GetParticlesManager());
		explosion->GetTransform().SetPosition(GetTransform().GetPosition());
		GetGameWorld()->Spawn(std::move(explosion));
		
		Despawn();
	}
	
	void RocketTurret::RenderProperties()
	{
		RenderBaseProperties(Transform::Properties::Position);
		
		bool resetRotation = false;
		
		if (ImGui::SliderAngle("Base Rotation", &m_baseRotation))
			resetRotation = true;
		
		if (ImGui::SliderAngle("Max Rotation", &m_maxRotationAngle, 0, 180))
		{
			m_maxRotationAngle = glm::clamp(m_maxRotationAngle, 0.0f, glm::pi<float>());
			m_minRotationAngle = -m_maxRotationAngle;
			resetRotation = true;
		}
		
		if (resetRotation)
			GetTransform().SetRotation(m_baseRotation);
		
		if (ImGui::InputFloat("Rotation Speed", &m_rotationSpeed))
			m_rotationSpeed = glm::max(m_rotationSpeed, 0.0f);
		
		if (ImGui::InputFloat("Fire Delay (s)", &m_fireDelay))
			m_fireDelay = glm::max(m_fireDelay, 0.0f);
	}
	
	const char* RocketTurret::GetObjectName() const
	{ return "Rocket Turret"; }
	
	const char* RocketTurret::GetSerializeClassName() const
	{ return "RocketTurret"; }
	
	nlohmann::json RocketTurret::Serialize() const
	{
		nlohmann::json json;
		
		json["transform"] = GetTransform().Serialize(Transform::Properties::Position);
		json["transform"]["rotation"] = glm::degrees(m_baseRotation);
		
		json["rotation_min"] = glm::degrees(m_minRotationAngle);
		json["rotation_max"] = glm::degrees(m_maxRotationAngle);
		json["rotation_speed"] = m_rotationSpeed;
		json["fire_delay"] = m_fireDelay;
		
		return json;
	}
	
	std::unique_ptr<Entity> RocketTurret::Clone() const
	{
		std::unique_ptr<RocketTurret> clone = std::make_unique<RocketTurret>(m_minRotationAngle, m_maxRotationAngle,
		                                                                     m_rotationSpeed);
		
		clone->GetTransform() = GetTransform();
		clone->GetTransform().SetRotation(m_baseRotation);
		
		return clone;
	}
}
