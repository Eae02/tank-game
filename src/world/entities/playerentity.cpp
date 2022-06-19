#include "playerentity.h"
#include "explosionentity.h"
#include "shieldentity.h"
#include "conveyorbeltentity.h"
#include "../spteams.h"
#include "../gameworld.h"
#include "../../settings.h"
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
	
	static constexpr float PLAYER_MAX_HP = 100;
	
	static constexpr uint64_t KEYS_MOVE_FORWARD = KeyToBitmask(Key::ArrowUp) | KeyToBitmask(Key::W);
	static constexpr uint64_t KEYS_MOVE_BACK    = KeyToBitmask(Key::ArrowDown) | KeyToBitmask(Key::S);
	static constexpr uint64_t KEYS_ROTATE_LEFT  = KeyToBitmask(Key::ArrowLeft) | KeyToBitmask(Key::A);
	static constexpr uint64_t KEYS_ROTATE_RIGHT = KeyToBitmask(Key::ArrowRight) | KeyToBitmask(Key::D);
	
	static constexpr Key KEY_INTERACT = Key::E;
	
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
	
	// ** Physics constants **
	
	//Maximum movement speed (in tiles / second)
	const float MAX_SPEED         = 3.0f;
	
	//The amount of time it takes for the tank to reach maximum speed (in seconds)
	const float ACCEL_TIME        = 0.15f;
	
	//Maximum rotational speed (in radians / second)
	const float ROT_MAX_SPEED     = 1.5f;
	
	//The amount of time it takes for the tank to reach maximum rotational speed (in seconds)
	const float ROT_ACCEL_TIME    = 0.1f;
	
	const float DRIFT_CANCEL_RATE = 10;
	
	//Number of frames to step the movement animation by for every tile moved
	const float ANIMATION_SPEED   = 15;
	
	const float ACCEL_AMOUNT = MAX_SPEED / ACCEL_TIME;
	const float ROT_ACCEL_AMOUNT = ROT_MAX_SPEED / ROT_ACCEL_TIME;
	
	void PlayerEntity::Update(const UpdateInfo& updateInfo)
	{
		m_powerUpState.Update(updateInfo.m_dt);
		
		if (updateInfo.m_keyboard.IsDown(Key::D1))
			m_weaponState.SelectPlasmaGun();
		else if (updateInfo.m_keyboard.IsDown(Key::D2))
			m_weaponState.SelectSpecialWeapon(SpecialWeapons::RocketLauncher);
		
		glm::vec2 forward = GetTransform().GetForward();
		glm::vec2 left(forward.y, -forward.x);
		
		glm::vec2 localVel(glm::dot(m_velocity, forward), glm::dot(m_velocity, left));
		glm::vec2 localAcc;
		
		if (localVel.y < 0)
		{
			localVel.y += updateInfo.m_dt * DRIFT_CANCEL_RATE;
			if (localVel.y > 0)
				localVel.y = 0;
		}
		if (localVel.y > 0)
		{
			localVel.y -= updateInfo.m_dt * DRIFT_CANCEL_RATE;
			if (localVel.y < 0)
				localVel.y = 0;
		}
		
		if (updateInfo.m_keyboard.IsDown(KEY_INTERACT) && !updateInfo.m_keyboard.WasDown(KEY_INTERACT))
		{
			GetGameWorld()->IterateIntersectingEntities(GetInteractRectangle(), [] (Entity& entity)
			{
				if (entity.CanInteract())
					entity.OnInteract();
			});
		}
		
		const bool moveForward = updateInfo.m_keyboard.IsAnyDown(KEYS_MOVE_FORWARD);
		const bool moveBack =    updateInfo.m_keyboard.IsAnyDown(KEYS_MOVE_BACK);
		const bool rotateLeft =  updateInfo.m_keyboard.IsAnyDown(KEYS_ROTATE_LEFT);
		const bool rotateRight = updateInfo.m_keyboard.IsAnyDown(KEYS_ROTATE_RIGHT);
		
		if (moveForward == moveBack)
		{
			if (localVel.x < 0)
			{
				localVel.x += updateInfo.m_dt * ACCEL_AMOUNT;
				if (localVel.x > 0)
					localVel.x = 0.0f;
			}
			if (localVel.x > 0)
			{
				localVel.x -= updateInfo.m_dt * ACCEL_AMOUNT;
				if (localVel.x < 0)
					localVel.x = 0.0f;
			}
		}
		else if (moveForward)
		{
			localAcc += glm::vec2(1, 0);
		}
		else //if (moveBack)
		{
			localAcc -= glm::vec2(1, 0);
		}
		
		float accelMag = glm::length(localAcc);
		if (accelMag > 1E-6f)
		{
			localAcc *= ACCEL_AMOUNT / accelMag;
			
			localVel += localAcc * updateInfo.m_dt;
		}
		
		//Caps the local velocity to the maximum speed
		float speed = glm::length(localVel);
		if (speed > MAX_SPEED)
		{
			localVel *= MAX_SPEED / speed;
		}
		
		float oldRotationVelocity = m_rotationVelocity;
		
		if (rotateLeft == rotateRight)
		{
			if (m_rotationVelocity < 0)
			{
				m_rotationVelocity += updateInfo.m_dt * ROT_ACCEL_AMOUNT;
				if (m_rotationVelocity > 0)
					m_rotationVelocity = 0.0f;
			}
			if (m_rotationVelocity > 0)
			{
				m_rotationVelocity -= updateInfo.m_dt * ROT_ACCEL_AMOUNT;
				if (m_rotationVelocity < 0)
					m_rotationVelocity = 0.0f;
			}
		}
		else if (rotateLeft)
		{
			m_rotationVelocity += 1.0f;
		}
		else //if (rotateRight)
		{
			m_rotationVelocity -= 1.0f;
		}
		
		float rotationSpeed = std::abs(m_rotationVelocity);
		if (rotationSpeed > ROT_MAX_SPEED)
		{
			m_rotationVelocity *= ROT_MAX_SPEED / rotationSpeed;
		}
		
		AdvanceFrame(-localVel.x * updateInfo.m_dt * ANIMATION_SPEED);
		
		glm::vec2 oldVelocity = m_velocity;
		m_velocity = localVel.x * forward + localVel.y * left;
		
		glm::vec2 groundVelocity = GetGameWorld()->GetGroundVelocity(GetTransform().GetPosition());
		glm::vec2 move = ((m_velocity + oldVelocity) * 0.5f + groundVelocity) * updateInfo.m_dt;
		
		//Updates the tank's position
		GetTransform().Rotate((m_rotationVelocity + oldRotationVelocity) * 0.5f * updateInfo.m_dt);
		GetTransform().Translate(move);
		
		//Collision correction
		Circle circle = GetTransform().GetBoundingCircle();
		circle.SetRadius(circle.GetRadius() * 0.8f);
		IntersectInfo intersectInfo = GetGameWorld()->GetIntersectInfo(circle, [] (const ICollidable& collidable)
		{
			return collidable.GetCollidableType() != CollidableTypes::Player;
		});
		if (intersectInfo.m_intersects)
			GetTransform().Translate(-intersectInfo.m_penetration);
		
		glm::vec2 screenMouseCoords = updateInfo.m_mouse.pos / glm::vec2(updateInfo.m_windowWidth, updateInfo.m_windowHeight);
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
		
		if (updateInfo.m_mouse.IsDown(MouseButton::Left) && CanFire(updateInfo.m_gameTime))
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
