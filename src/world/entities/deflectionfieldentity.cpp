#include "deflectionfieldentity.h"
#include "../../updateinfo.h"

#include <imgui.h>

namespace TankGame
{
	static const float MIN_INTENSITY = 10;
	static const float MAX_INTENSITY = 25;
	
	DeflectionFieldEntity::DeflectionFieldEntity(float length)
	    : RayLightEntity(ParseColorHexCodeSRGB(0x49B9DE), MIN_INTENSITY, Attenuation(0, 10), glm::max(length, 0.0f), 0.05f)
	{
		
	}
	
	void DeflectionFieldEntity::Update(const UpdateInfo& updateInfo)
	{
		if (m_flashTime < 0)
			return;
		
		m_flashTime -= updateInfo.m_dt * 5;
		
		SetIntensity(glm::mix(MIN_INTENSITY, MAX_INTENSITY, glm::max(m_flashTime, 0.0f)));
	}
	
	std::unique_ptr<Entity> DeflectionFieldEntity::Clone() const
	{
		std::unique_ptr<DeflectionFieldEntity> clone = std::make_unique<DeflectionFieldEntity>(GetLength());
		
		clone->GetTransform() = GetTransform();
		clone->m_deflectEnemy = m_deflectEnemy;
		
		return clone;
	}
	
	void DeflectionFieldEntity::RenderProperties()
	{
		RenderBaseProperties(Transform::Properties::Position | Transform::Properties::Rotation);
		
		float length = GetLength();
		if (ImGui::InputFloat("Length", &length))
			SetLength(glm::max(length, 0.0f));
		
		ImGui::Checkbox("Deflect Enemy's Bullets", &m_deflectEnemy);
	}
	
	const char* DeflectionFieldEntity::GetObjectName() const
	{
		return "Deflection Field";
	}
	
	const char* DeflectionFieldEntity::GetSerializeClassName() const
	{
		return "DeflectionField";
	}
	
	nlohmann::json DeflectionFieldEntity::Serialize() const
	{
		nlohmann::json json;
		
		json["transform"] = GetTransform().Serialize(Transform::Properties::Position | Transform::Properties::Rotation);
		
		if (m_deflectEnemy)
			json["deflect_enemy"] = m_deflectEnemy;
		
		json["length"] = GetLength();
		
		return json;
	}
	
	ColliderInfo DeflectionFieldEntity::GetColliderInfo() const
	{
		glm::vec2 forward = GetTransform().GetForward();
		
		return OrientedRectangle(GetTransform().GetPosition() + forward * GetLength() * 0.5f,
		                         glm::vec2(0.1f, GetLength() / 2), GetTransform().GetRotation());
	}
	
	CollidableTypes DeflectionFieldEntity::GetCollidableType() const
	{
		return CollidableTypes::Npc;
	}
}
