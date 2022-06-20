#include "rocketentity.h"
#include "../explosionentity.h"
#include "../../gameworld.h"
#include "../../../audio/soundsmanager.h"
#include "../../../utils/utils.h"
#include "../../../utils/ioutils.h"
#include "../../../utils/mathutils.h"
#include "../../../graphics/spriterenderlist.h"
#include "../../../updateinfo.h"

namespace TankGame
{
	static const float SIZE = 0.15f;
	
	std::unique_ptr<Texture2D> RocketEntity::s_diffuse;
	std::unique_ptr<Texture2D> RocketEntity::s_normalMap;
	std::unique_ptr<SpriteMaterial> RocketEntity::s_material;
	
	RocketEntity::RocketEntity(ParticlesManager& particlesManager, int teamID, const Entity* sourceEntity, float damage)
	    : PointLightEntity(ParseColorHexCodeSRGB(0xff6c33), 4.0f, Attenuation(0, 0.5f)),
	      ProjectileEntity(teamID, sourceEntity, 10, damage, 0.5f),
	      ParticleSystemEntity(SmokeParticleSystem(particlesManager)),
	      m_audioSource(AudioSource::VolumeModes::Effect)
	{
		if (s_material== nullptr)
		{
			s_diffuse = std::make_unique<Texture2D>(Texture2D::FromFile(resDirectoryPath / "rocket-diffuse.png"));
			s_normalMap = std::make_unique<Texture2D>(Texture2D::FromFile(resDirectoryPath / "rocket-normals.png"));
			
			s_diffuse->SetWrapMode(GL_CLAMP_TO_EDGE);
			s_normalMap->SetWrapMode(GL_CLAMP_TO_EDGE);
			
			s_material = std::make_unique<SpriteMaterial>(*s_diffuse, *s_normalMap, 1, 30);
			
			CallOnClose([] { s_diffuse = nullptr; s_normalMap = nullptr; s_material = nullptr; });
		}
		
		m_spriteTransform = GetTransform();
		
		float textureAR = s_diffuse->GetHeight() / static_cast<float>(s_diffuse->GetWidth());
		m_spriteTransform.SetScale({ SIZE, SIZE * textureAR });
		m_spriteTransform.SetCenterOfRotation({ 0, 1 });
		
		m_audioSource.SetBuffer(SoundsManager::GetInstance().GetSound("RocketFire"));
	}
	
	void RocketEntity::Draw(SpriteRenderList& spriteRenderList) const
	{
		m_spriteTransform.SetPosition(GetTransform().GetPosition());
		m_spriteTransform.SetRotation(GetTransform().GetRotation());
		
		spriteRenderList.Add(m_spriteTransform, *s_material, 0.2f);
	}
	
	void RocketEntity::Update(const UpdateInfo& updateInfo)
	{
		ProjectileEntity::Update(updateInfo);
		m_audioSource.SetPosition(GetTransform().GetPosition());
	}
	
	void RocketEntity::OnSpawned(GameWorld& gameWorld)
	{
		m_audioSource.Play();
		
		PointLightEntity::OnSpawned(gameWorld);
		ParticleSystemEntity::OnSpawned(gameWorld);
	}
	
	void RocketEntity::OnImpact(ImpactFlags flags, glm::vec2 penetration)
	{
		auto explosion = std::make_unique<ExplosionEntity>(GetGameWorld()->GetParticlesManager());
		explosion->GetTransform().SetPosition(GetTransform().GetPosition() - GetTransform().GetForward() * 0.1f);
		GetGameWorld()->Spawn(std::move(explosion));
		
		m_audioSource.Stop();
		
		Despawn();
	}
	
	const char* RocketEntity::GetSerializeClassName() const
	{
		return nullptr;
	}
	
	std::unique_ptr<Entity> RocketEntity::Clone() const
	{
		return nullptr;
	}
}
