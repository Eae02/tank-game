#include "activatorentity.h"
#include "../gameworld.h"
#include "../../audio/soundeffectplayer.h"

#include <imgui.h>

namespace TankGame
{
	ActivatorEntity::ActivatorEntity(std::string onActivateEvent, bool singleUse)
	    : PropEntity("Activator"), m_onActivateEvent(std::move(onActivateEvent)), m_singleUse(singleUse)
	{
		SetZ(0.6f);
		SetSizeX(0.5f);
	}
	
	bool ActivatorEntity::CanInteract() const
	{
		return !((m_singleUse && m_hasActivated) || m_onActivateEvent.empty());
	}
	
	void ActivatorEntity::OnInteract()
	{
		GetGameWorld()->SendEvent(m_onActivateEvent, this);
		m_hasActivated = true;
	}
	
	const char* ActivatorEntity::GetObjectName() const
	{ return "Activator"; }
	
	const char* ActivatorEntity::GetSerializeClassName() const
	{ return "Activator"; }
	
	void ActivatorEntity::RenderProperties()
	{
		RenderTransformProperty(Transform::Properties::Position);
		
		ImGui::Checkbox("Single Use", &m_singleUse);
		
		std::array<char, 256> buffer;
		buffer.back() = '\n';
		strncpy(buffer.data(), m_onActivateEvent.c_str(), buffer.size());
		
		if (ImGui::InputText("Activate Event", buffer.data(), buffer.size()))
			m_onActivateEvent = buffer.data();
	}
	
	nlohmann::json ActivatorEntity::Serialize() const
	{
		nlohmann::json json;
		
		json["transform"] = GetTransform().Serialize(Transform::Properties::Position);
		json["single_use"] = m_singleUse;
		json["activated"] = m_onActivateEvent;
		
		return json;
	}
	
	std::unique_ptr<Entity> ActivatorEntity::Clone() const
	{
		return std::make_unique<ActivatorEntity>(*this);
	}
}
