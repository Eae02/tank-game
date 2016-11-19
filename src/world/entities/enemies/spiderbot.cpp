#include "spiderbot.h"
#include "../playerentity.h"
#include "../explosionentity.h"
#include "../hppickupentity.h"
#include "../../props/propclass.h"
#include "../../props/propsmanager.h"
#include "../../pathfinder.h"
#include "../../gameworld.h"
#include "../../spteams.h"
#include "../../../settings.h"
#include "../../../audio/soundeffectplayer.h"
#include "../../../updateinfo.h"
#include "../../../ai/aiutils.h"
#include "../../../utils/mathutils.h"
#include "../../../utils/utils.h"
#include "../../../graphics/spriterenderlist.h"

#include <GLFW/glfw3.h>

namespace TankGame
{
	static const float CIRCLE_RADIUS = 0.4f;
	static const float BODY_SIZE = 0.3f;
	static const float LEG_SIZE = 0.12f;
	
	static const float EXPLOSION_DIST = 1;
	
	static SoundEffectPlayer soundEffectPlayer{ "SpiderWakingUp" };
	
	SpiderBot::SpiderBot()
	    : PointLightEntity(ParseColorHexCodeSRGB(0xE8153C), 0, { 0, 0.7f }), Hittable(30, EnemyTeamID),
	      m_bodyPropClass(*PropsManager::GetInstance().GetPropClassByName("SpiderbotBody")),
	      m_legsPropClass(*PropsManager::GetInstance().GetPropClassByName("SpiderbotLeg"))
	{
		float legAR = m_legsPropClass.GetTextureHeight() / static_cast<float>(m_legsPropClass.GetTextureWidth());
		m_legSize = { LEG_SIZE * 0.75f, LEG_SIZE * legAR };
		
		if (Settings::GetInstance().GetLightingQuality() != QualitySettings::Low)
			SetShadowMode(EntityShadowModes::Dynamic);
		else
			SetShadowMode(EntityShadowModes::None);
		
		SetSolidType(SolidTypes::Npc);
	}
	
	void SpiderBot::Draw(SpriteRenderList& spriteRenderList) const
	{
		float rotationOff = (std::abs(glm::fract(m_animationTime) - 0.5f) * 4 - 1) * 0.75f;
		
		Transform bodyTransform;
		bodyTransform.SetScale({ BODY_SIZE, BODY_SIZE });
		bodyTransform.SetPosition(GetTransform().GetPosition());
		bodyTransform.SetRotation(GetTransform().GetRotation() + rotationOff * 0.4f);
		
		spriteRenderList.Add(bodyTransform, m_bodyPropClass.GetMaterial(), 0.6f);
		
		const float legRotations[] = 
		{
			glm::radians(90.0f - 40.0f),
			glm::radians(90.0f + 40.0f),
			glm::radians(270.0f - 40.0f),
			glm::radians(270.0f + 40.0f)
		};
		
		for (int i = 0; i < ArrayLength(legRotations); i++)
		{
			float legRotation = GetTransform().GetRotation() + legRotations[i];
			glm::vec2 legDirection = RotateVector({ 0, -1 }, legRotation);
			
			Transform transform;
			transform.SetPosition(GetTransform().GetPosition() + legDirection * 0.2f * m_wakeupAnimationTime);
			transform.SetRotation(legRotation + rotationOff);
			transform.SetCenterOfRotation({ 0, 1 });
			transform.SetScale(m_legSize);
			
			spriteRenderList.Add(transform, m_legsPropClass.GetMaterial(), 0.65f);
		}
	}
	
	void SpiderBot::Update(const UpdateInfo& updateInfo)
	{
		if (m_player == nullptr)
			return;
		
		if (m_isAwake)
		{
			float intensity = std::sin(updateInfo.m_gameTime * 30) * 3 + 7;
			
			if (m_wakeupAnimationTime < 1)
			{
				m_wakeupAnimationTime += updateInfo.m_dt * 2.5f;
				SetIntensity(intensity * m_wakeupAnimationTime);
				return;
			}
			
			float distToPlayer = glm::distance(m_player->GetTransform().GetPosition(), GetTransform().GetPosition());
			
			if (distToPlayer < EXPLOSION_DIST)
			{
				OnKilled();
				return;
			}
			
			double time = glfwGetTime();
			if (time > m_pathToPlayerNextUpdateTime)
			{
				m_pathToPlayer = Path();
				
				if (!PathFinder::GetInstance().FindPath(*GetGameWorld(), GetTransform().GetPosition(), 
				                                        m_player->GetTransform().GetPosition(), m_pathToPlayer,
				                                        CIRCLE_RADIUS))
				{
					OnKilled();
					return;
				}
				
				m_pathToPlayerNextUpdateTime = time + 0.2;
				m_pathToPlayerProgress = 0.0f;
			}
			
			SetIntensity(intensity);
			
			WalkPath(updateInfo.m_dt, m_pathToPlayer, m_pathToPlayerProgress, GetTransform(),
			         3.0f, glm::radians(180.0f), true);
			
			m_animationTime += updateInfo.m_dt * 3;
		}
		else
		{
			const float DETECT_DISTANCE = 10;
			
			glm::vec2 toPlayer = m_player->GetTransform().GetPosition() - GetTransform().GetPosition();
			
			if (LengthSquared(toPlayer) < DETECT_DISTANCE * DETECT_DISTANCE &&
			    !GetGameWorld()->IsRayObstructed(GetTransform().GetPosition(), m_player->GetTransform().GetPosition()))
			{
				m_isAwake = true;
				soundEffectPlayer.Play(GetTransform().GetPosition(), 1.0f, 1.0f, 0.2f);
			}
		}
	}
	
	void SpiderBot::OnSpawned(GameWorld& gameWorld)
	{
		m_player = dynamic_cast<PlayerEntity*>(gameWorld.GetEntityByName("player"));
		
		PointLightEntity::OnSpawned(gameWorld);
	}
	
	Circle SpiderBot::GetHitCircle() const
	{
		return Circle(GetTransform().GetPosition(), CIRCLE_RADIUS);
	}
	
	void SpiderBot::OnKilled()
	{
		auto explosion = std::make_unique<ExplosionEntity>(GetGameWorld()->GetParticlesManager());
		explosion->GetTransform().SetPosition(GetTransform().GetPosition());
		GetGameWorld()->Spawn(std::move(explosion));
		
		if (m_player != nullptr)
		{
			float distToPlayer = glm::distance(m_player->GetTransform().GetPosition(), GetTransform().GetPosition());
			if (distToPlayer < EXPLOSION_DIST)
			{
				m_player->SetHp(m_player->GetHp() - 45);
			}
			else if (GetHp() == 0)
			{
				m_player->GiveEnergy();
				if (m_player != nullptr && m_player->GetHp() < m_player->GetMaxHp())
					HpPickupEntity::SpawnEntities(*GetGameWorld(), GetTransform().GetPosition(), 15.0f);
			}
		}
		
		Despawn();
	}
	
	void SpiderBot::RenderProperties()
	{
		RenderTransformProperty(Transform::Properties::Position | Transform::Properties::Rotation);
	}
	
	const char* SpiderBot::GetObjectName() const
	{ return "Spider Bot"; }
	const char* SpiderBot::GetSerializeClassName() const
	{ return "SpiderBot"; }
	
	nlohmann::json SpiderBot::Serialize() const
	{
		nlohmann::json json;
		json["transform"] = GetTransform().Serialize(Transform::Properties::Position | Transform::Properties::Rotation);
		return json;
	}
	
	std::unique_ptr<Entity> SpiderBot::Clone() const
	{
		std::unique_ptr<SpiderBot> clone = std::make_unique<SpiderBot>();
		clone->GetTransform() = GetTransform();
		return clone;
	}
}
