#include "enemytank.h"
#include "../playerentity.h"
#include "../explosionentity.h"
#include "../hppickupentity.h"
#include "../../spteams.h"
#include "../../gameworld.h"
#include "../../../tanktextures.h"
#include "../../../updateinfo.h"
#include "../../../utils/utils.h"
#include "../../../utils/ioutils.h"
#include "../../../graphics/spritematerial.h"

#include <imgui.h>

namespace TankGame
{
	bool EnemyTank::s_areTexturesLoaded = false;
	
	StackObject<Texture2D> EnemyTank::s_cannonTexture;
	StackObject<Texture2D> EnemyTank::s_cannonNormalMap;
	StackObject<SpriteMaterial> EnemyTank::s_cannonMaterial;
	
	static const TankEntity::TextureInfo textureInfo =
	{
		/* m_baseTextureWidth    */ 126,
		/* m_baseTextureHeight   */ 181,
		/* m_baseTextureFrames   */ 9,
		/* m_cannonTextureWidth  */ 36,
		/* m_cannonTextureHeight */ 74,
		/* m_cannonYOffset       */ -0.1f
	};
	
	EnemyTank::EnemyTank(const Path& idlePath)
	    : TankEntity(glm::vec3(1.0f), textureInfo, EnemyTeamID, 50),
	      m_ai(*this, { 2.0f, GetTransform().GetBoundingCircle().GetRadius() }, idlePath)
	{
		SetIsRocketTank(false);
		
		SetFireCooldown(0.5f);
		
		GetTransform().SetPosition(idlePath[0]);
		
		CannonMaterial();
		
		SetSolidType(SolidTypes::Npc);
	}
	
	void EnemyTank::SetIsRocketTank(bool isRocketTank)
	{
		if (isRocketTank)
		{
			SetColor(ParseColorHexCodeSRGB(0xE86E15));
		}
		else
		{
			SetColor(ParseColorHexCodeSRGB(0xE8153C));
		}
		
		m_isRocketTank = isRocketTank;
	}
	
	void EnemyTank::Update(const UpdateInfo& updateInfo)
	{
		if (const Entity* player = GetGameWorld()->GetEntityByName("player"))
			m_ai.Update(player->GetTransform().GetPosition(), updateInfo);
		else
			m_ai.Update({ 0.0f, 0.0f }, updateInfo);
		
		TankEntity::Update(updateInfo);
	}
	
	const SpriteMaterial& EnemyTank::GetBaseMaterial(int frame) const
	{
		return TankTextures::GetInstance().GetBaseMaterial(frame);
	}
	
	const SpriteMaterial& EnemyTank::CannonMaterial()
	{
		if (!s_areTexturesLoaded)
		{
			fs::path texturePath = GetResDirectory() / "tank" / "enemy";
			s_cannonTexture.Construct(Texture2D::FromFile(texturePath / "cannon.png"));
			s_cannonNormalMap.Construct(Texture2D::FromFile(texturePath / "cannon-normals.png"));
			s_cannonMaterial.Construct(*s_cannonTexture, *s_cannonNormalMap, 1, 30);
			
			s_cannonTexture->SetWrapMode(GL_CLAMP_TO_EDGE);
			s_cannonNormalMap->SetWrapMode(GL_CLAMP_TO_EDGE);
			
			CallOnClose([]
			{
				s_cannonTexture.Destroy();
				s_cannonNormalMap.Destroy();
				s_cannonMaterial.Destroy();
				
				s_areTexturesLoaded = false;
			});
			
			s_areTexturesLoaded = true;
		}
		
		return *s_cannonMaterial;
	}
	
	Transform EnemyTank::GetBaseCannonTransform()
	{
		return TankEntity::GetBaseCannonTransform(textureInfo);
	}
	
	void EnemyTank::OnSpawned(GameWorld& gameWorld)
	{
		if (!m_detectPlayerEventName.empty())
			gameWorld.ListenForEvent(m_detectPlayerEventName, *this);
		m_ai.SetGameWorld(&gameWorld);
		m_oldPosition = GetTransform().GetPosition();
		SpotLightEntity::OnSpawned(gameWorld);
	}
	
	Circle EnemyTank::GetHitCircle() const
	{
		return GetTransform().GetBoundingCircle();
	}
	
	const SpriteMaterial& EnemyTank::GetCannonMaterial() const
	{
		return *s_cannonMaterial;
	}
	
	void EnemyTank::OnKilled()
	{
		auto explosion = std::make_unique<ExplosionEntity>(GetGameWorld()->GetParticlesManager());
		explosion->GetTransform().SetPosition(GetTransform().GetPosition());
		GetGameWorld()->Spawn(std::move(explosion));
		
		const PlayerEntity* player = dynamic_cast<const PlayerEntity*>(GetGameWorld()->GetEntityByName("player"));
		if (player != nullptr && player->GetHp() < player->GetMaxHp())
			HpPickupEntity::SpawnEntities(*GetGameWorld(), GetTransform().GetPosition(), 20.0f);
		
		Despawn();
	}
	
	const char* EnemyTank::GetObjectName() const
	{
		return "Enemy Tank";
	}
	
	std::unique_ptr<Entity> EnemyTank::Clone() const
	{
		std::unique_ptr<EnemyTank> clone = std::make_unique<EnemyTank>(m_ai.GetIdlePath());
		clone->GetTransform() = GetTransform();
		return std::move(clone);
	}
	
	const char* EnemyTank::GetSerializeClassName() const
	{
		return "EnemyTank";
	}
	
	nlohmann::json EnemyTank::Serialize() const
	{
		nlohmann::json json = Entity::Serialize();
		
		const Path& idlePath = m_ai.GetIdlePath();
		nlohmann::json::array_t idlePathEl;
		for (size_t i = 0; i < idlePath.GetNodeCount(); i++)
		{
			idlePathEl.push_back(nlohmann::json::array({ idlePath[i].x, idlePath[i].y }));
		}
		
		json["idle_path"] = idlePathEl;
		
		if (m_isRocketTank)
			json["rocket_tank"] = true;
		
		if (!m_detectPlayerEventName.empty())
			json["detect_event"] = m_detectPlayerEventName;
		
		return json;
	}
	
	void EnemyTank::HandleEvent(const std::string& event, Entity* sender)
	{
		if (event == "EditorSpawned")
		{
			Path idlePath;
			
			idlePath.AddNode(GetTransform().GetPosition());
			idlePath.AddNode(GetTransform().GetPosition() + glm::vec2(0, 1));
			idlePath.AddNode(GetTransform().GetPosition() + glm::vec2(1, 1));
			idlePath.AddNode(GetTransform().GetPosition() + glm::vec2(1, 0));
			idlePath.Close();
			
			m_ai.SetIdlePath(std::move(idlePath));
		}
		else if (event == "EditorMoved")
		{
			glm::vec2 deltaMove = GetTransform().GetPosition() - m_oldPosition;
			m_oldPosition = GetTransform().GetPosition();
			
			//Moves the idle path when the entity is moved in the editor
			Path& path = m_ai.GetIdlePath();
			for (size_t i = 0; i < path.GetNodeCount(); i++)
				path[i] += deltaMove;
			m_ai.IdlePathChanged();
		}
		else if (event == m_detectPlayerEventName)
		{
			if (const Entity* player = GetGameWorld()->GetEntityByName("player"))
				m_ai.DetectPlayer(player->GetTransform().GetPosition());
		}
	}
	
	Path& EnemyTank::GetEditPath()
	{
		return m_ai.GetIdlePath();
	}
	
	void EnemyTank::PathEditEnd()
	{
		m_ai.IdlePathChanged();
	}
	
	const char* EnemyTank::GetEditPathName() const
	{
		return "Idle Path";
	}
	
	void EnemyTank::RenderProperties()
	{
		TankEntity::RenderProperties();
		
		bool isRocketTank = m_isRocketTank;
		if (ImGui::Checkbox("Rocket Tank", &isRocketTank))
			SetIsRocketTank(isRocketTank);
		
		std::array<char, 256> inputBuffer;
		inputBuffer.back() = '\0';
		strncpy(inputBuffer.data(), m_detectPlayerEventName.c_str(), inputBuffer.size() - 1);
		
		if (ImGui::InputText("Detect Event", inputBuffer.data(), inputBuffer.size()))
			m_detectPlayerEventName = inputBuffer.data();
	}
}
