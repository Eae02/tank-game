#include "playerentity.h"
#include "explosionentity.h"
#include "shieldentity.h"
#include "conveyorbeltentity.h"
#include "../spteams.h"
#include "../gameworld.h"
#include "../../settings.h"
#include "../../inpututils.h"
#include "../../tanktextures.h"
#include "../../audio/soundsmanager.h"
#include "../../updateinfo.h"
#include "../../graphics/spriterenderlist.h"
#include "../../graphics/imainrenderer.h"
#include "../../keyboard.h"
#include "../../mouse.h"
#include "../../utils/mathutils.h"
#include "../../utils/utils.h"
#include "../../utils/ioutils.h"
#include "../../gamemanager.h"
#include "../../lua/luavm.h"

#include <glm/gtc/color_space.hpp>

namespace TankGame
{
	constexpr float PlayerEntity::MAX_ENERGY;
	
	bool PlayerEntity::s_areTexturesLoaded = false;
	
	std::unique_ptr<Texture2D> PlayerEntity::s_cannonTexture;
	std::unique_ptr<Texture2D> PlayerEntity::s_cannonNormalMap;
	std::unique_ptr<SpriteMaterial> PlayerEntity::s_cannonMaterial;
	
	static std::uniform_real_distribution<float> s_energyUsageDist(4.0f, 8.0f);
	
	const TankEntity::TextureInfo textureInfo =
	{
			/* m_baseTextureWidth    */ 126,
			/* m_baseTextureHeight   */ 181,
			/* m_baseTextureFrames   */ 9,
			/* m_cannonTextureWidth  */ 36,
			/* m_cannonTextureHeight */ 74,
			/* m_cannonYOffset       */ -0.1f
	};
	
	const float PLAYER_MAX_HP = 100;
	
	PlayerEntity::PlayerEntity()
	    : TankEntity(ParseColorHexCodeSRGB(0xFADD98), textureInfo, PlayerTeamID, PLAYER_MAX_HP),
	      m_noAmmoAudioSource(AudioSource::VolumeModes::Effect), m_dist(0.0f, 1.0f)
	{
		if (!s_areTexturesLoaded)
		{
			fs::path texturePath = GetResDirectory() / "tank" / "player";
			s_cannonTexture = std::make_unique<Texture2D>(Texture2D::FromFile(texturePath / "cannon.png"));
			s_cannonNormalMap = std::make_unique<Texture2D>(Texture2D::FromFile(texturePath / "cannon-normals.png"));
			s_cannonMaterial = std::make_unique<SpriteMaterial>(*s_cannonTexture, *s_cannonNormalMap, 1, 30);
			
			s_cannonTexture->SetWrapMode(GL_CLAMP_TO_EDGE);
			s_cannonNormalMap->SetWrapMode(GL_CLAMP_TO_EDGE);
			
			CallOnClose([]
			{
				s_cannonTexture = nullptr;
				s_cannonNormalMap = nullptr;
				s_cannonMaterial = nullptr;
				
				s_areTexturesLoaded = false;
			});
			
			s_areTexturesLoaded = true;
		}
		
		m_noAmmoAudioSource.SetVolume(0.7f);
		m_noAmmoAudioSource.SetBuffer(SoundsManager::GetInstance().GetSound("NoAmmo"));
	}
	
	void PlayerEntity::Update(const UpdateInfo& updateInfo)
	{
		m_powerUpState.Update(updateInfo.m_dt);
		
		if (updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_1))
			m_weaponState.SelectPlasmaGun();
		else if (updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_2))
			m_weaponState.SelectSpecialWeapon(SpecialWeapons::RocketLauncher);
		
		glm::vec2 forward = GetTransform().GetForward();
		
		glm::vec2 force(0.0f);
		float rotForce = 0.0f;
		
		const float FRAME_MULTIPLIER = 1.7f;
		const float MOVE_FORCE = 40;
		
		const float ROTATION_FORCE = 20;
		
		const float DRAG_CONSTANT = 12;
		const float MASS = 0.8f;
		
		const Settings& settings = Settings::GetInstance();
		
		if (IsButtonPressedNow(updateInfo, settings.GetInteractButton()))
		{
			GetGameWorld()->IterateIntersectingEntities(GetInteractRectangle(), [] (Entity& entity)
			{
				if (entity.CanInteract())
					entity.OnInteract();
			});
		}
		
		if (IsButtonPressed(updateInfo, settings.GetForwardButton()))
		{
			force += forward * MOVE_FORCE;
			AdvanceFrame(-updateInfo.m_dt * FRAME_MULTIPLIER * MOVE_FORCE);
		}
		
		if (IsButtonPressed(updateInfo, settings.GetBackButton()))
		{
			force -= forward * MOVE_FORCE;
			AdvanceFrame(updateInfo.m_dt * FRAME_MULTIPLIER * MOVE_FORCE);
		}
		
		if (IsButtonPressed(updateInfo, settings.GetLeftButton()))
			rotForce += ROTATION_FORCE;
		if (IsButtonPressed(updateInfo, settings.GetRightButton()))
			rotForce -= ROTATION_FORCE;
		
		//Applies drag
		force -= m_velocity * DRAG_CONSTANT;
		rotForce -= m_rotationVelocity * DRAG_CONSTANT;
		
		glm::vec2 oldVelocity = m_velocity;
		float oldRotVelocity = m_rotationVelocity;
		
		//Updates the tank's velocity
		m_velocity += (force / MASS) * updateInfo.m_dt;
		m_rotationVelocity += (rotForce / MASS) * updateInfo.m_dt;
		
		
		
		//Caps the speed of the tank, this is to stop physics from breaking at low framerates
		const float MAX_SPEED = 2.5f;
		const float MAX_ROT_SPEED = 2;
		float speed = glm::length(m_velocity);
		if (speed > MAX_SPEED)
			m_velocity *= MAX_SPEED / speed;
		float rotSpeed = glm::abs(m_rotationVelocity);
		if (rotSpeed > MAX_ROT_SPEED)
			m_rotationVelocity *= MAX_ROT_SPEED / rotSpeed;
		
		glm::vec2 groundVelocity = GetGameWorld()->GetGroundVelocity(GetTransform().GetPosition());
		
		//Updates the tank's position
		GetTransform().Rotate((m_rotationVelocity + oldRotVelocity) * 0.5f * updateInfo.m_dt);
		GetTransform().Translate(((m_velocity + oldVelocity) * 0.5f + groundVelocity) * updateInfo.m_dt);
		
		//Collision correction
		Circle circle = GetTransform().GetBoundingCircle();
		circle.SetRadius(circle.GetRadius() * 0.8f);
		IntersectInfo intersectInfo = GetGameWorld()->GetIntersectInfo(circle, [] (const ICollidable& collidable)
		{
			return collidable.GetCollidableType() != CollidableTypes::Player;
		});
		if (intersectInfo.m_intersects)
			GetTransform().Translate(-intersectInfo.m_penetration);
		
		glm::vec2 screenMouseCoords = updateInfo.m_mouse.GetPosition() / glm::vec2(updateInfo.m_windowWidth, updateInfo.m_windowHeight);
		glm::vec2 mouseWorldPos = updateInfo.m_viewInfo.ScreenToWorld(screenMouseCoords);
		
		//Updates the cannon's rotation
		glm::vec2 pos = GetTransform().GetPosition();
		SetCannonRotation(glm::half_pi<float>() + std::atan2(mouseWorldPos.y - pos.y, mouseWorldPos.x - pos.x));
		
		//Regenerates energy
		m_energyRegenTime += updateInfo.m_dt;
		if (m_energyRegenTime > 2)
		{
			m_energyRegenTime = 2;
			m_energy = glm::min(m_energy + updateInfo.m_dt * 15, 100.0f);
		}
		
		if (IsButtonPressed(updateInfo, settings.GetFireButton()) && CanFire(updateInfo.m_gameTime))
			FireSelectedWeapon(updateInfo.m_gameTime);
		
		TankEntity::Update(updateInfo);
	}
	
	void PlayerEntity::FireSelectedWeapon(float gameTime)
	{
		FireParameters fireParams;
		fireParams.m_homing = HasPowerUp(PowerUps::HomingBullets);
		
		if (m_weaponState.IsUsingPlasmaGun())
		{
			float fireCost = s_energyUsageDist(randomGen);
			bool hasEnoughEnergy = true;
			
			if (fireCost > m_energy)
			{
				if (m_energy > s_energyUsageDist.min())
					fireCost = m_energy;
				else
					hasEnoughEnergy = false;
			}
			
			if (hasEnoughEnergy)
			{
				fireParams.m_rotationOffset = m_dist(randomGen) * glm::length(m_velocity) * 0.03f;
				
				FirePlasmaGun(ParseColorHexCodeSRGB(0x50FF4A), 10, gameTime, fireParams);
				
				m_energy -= fireCost;
				m_energyRegenTime = 0;
			}
		}
		else
		{
			if (m_weaponState.GetAmmoCount(m_weaponState.GetCurrentSpecialWeapon()) == 0)
			{
				GetGameWorld()->GetGameManager()->ShowNoAmmoText();
				
				if (!m_noAmmoAudioSource.IsPlaying())
					m_noAmmoAudioSource.Play();
			}
			else
			{
				switch (m_weaponState.GetCurrentSpecialWeapon())
				{
				case SpecialWeapons::RocketLauncher:
					FireRocket(50, gameTime, fireParams);
					break;
				}
				
				m_weaponState.GiveAmmo(m_weaponState.GetCurrentSpecialWeapon(), -1);
			}
		}
	}
	
	Circle PlayerEntity::GetHitCircle() const
	{
		Circle circle = GetTransform().GetBoundingCircle();
		circle.SetRadius(0.8f * circle.GetRadius());
		return circle;
	}
	
	void PlayerEntity::SetHp(float hp)
	{
		if (hp < GetHp())
		{
			GetGameWorld()->ShakeCamera(0.25f, 0.01f * (GetHp() - hp));
			if (IMainRenderer* mainRenderer = GetGameWorld()->GetRenderer())
				mainRenderer->DoDamageFlash();
		}
		
		Hittable::SetHp(hp);
	}
	
	const class SpriteMaterial& PlayerEntity::GetBaseMaterial(int frame) const
	{
		return TankTextures::GetInstance().GetBaseMaterial(frame);
	}
	
	const class SpriteMaterial& PlayerEntity::GetCannonMaterial() const
	{
		return *s_cannonMaterial;
	}
	
	void PlayerEntity::OnKilled()
	{
		TankEntity::OnKilled();
		
		Hittable::SetHp(GetMaxHp());
		m_energy = MAX_ENERGY;
		
		auto explosion = std::make_unique<ExplosionEntity>(GetGameWorld()->GetParticlesManager());
		explosion->GetTransform().SetPosition(GetTransform().GetPosition());
		GetGameWorld()->Spawn(std::move(explosion));
		
		Lua::DoString("if onPlayerKilled then onPlayerKilled() end", GetGameWorld()->GetLuaSandbox());
		
		GetTransform().SetPosition(GetGameWorld()->GetRespawnPosition());
		GetTransform().SetRotation(GetGameWorld()->GetRespawnRotation());
	}
	
	Rectangle PlayerEntity::GetInteractRectangle() const
	{
		const float INTERACT_DIST = 0.5f;
		return Rectangle::CreateCentered(GetTransform().GetPosition(), INTERACT_DIST, INTERACT_DIST);
	}
	
	CollidableTypes PlayerEntity::GetCollidableType() const
	{
		return CollidableTypes::Player;
	}
	
	const char* PlayerEntity::GetObjectName() const
	{
		return "Player";
	}
	
	void PlayerEntity::RenderProperties()
	{
		RenderBaseProperties(Transform::Properties::Position | Transform::Properties::Rotation, false);
	}
	
	void PlayerEntity::GiveEnergy()
	{
		std::uniform_real_distribution<float> dist(30.0f, 50.0f);
		m_energy = glm::min(m_energy + dist(randomGen), MAX_ENERGY);
	}
}
