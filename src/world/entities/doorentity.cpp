#include "doorentity.h"
#include "../gameworld.h"
#include "../../audio/soundeffectplayer.h"

#include <imgui.h>

namespace TankGame
{
	static SoundEffectPlayer soundEffectPlayer{ "DoorOpen" };
	
	DoorEntity::DoorEntity(std::string openEventName)
	    : PropEntity("Door", true), m_openEventName(std::move(openEventName))
	{
		SetSizeX(0.5f);
	}
	
	void DoorEntity::OnSpawned(GameWorld& gameWorld)
	{
		if (!m_openEventName.empty())
			gameWorld.ListenForEvent(m_openEventName, *this);
		PropEntity::OnSpawned(gameWorld);
	}
	
	void DoorEntity::HandleEvent(const std::string& event, Entity* sender)
	{
		if (event == m_openEventName)
			Open();
		PropEntity::HandleEvent(event, sender);
	}
	
	void DoorEntity::Open()
	{
		soundEffectPlayer.Play(GetTransform().GetPosition(), 1.0f, 1.0f, 0.5f);
		Despawn();
	}
	
	const char* DoorEntity::GetObjectName() const
	{ return "Door"; }
	const char* DoorEntity::GetSerializeClassName() const
	{ return "Door"; }
	
	void DoorEntity::RenderProperties()
	{
		RenderTransformProperty(Transform::Properties::Position | Transform::Properties::Rotation);
		
		float size = GetSizeX();
		if (ImGui::InputFloat("Size", &size))
			SetSizeX(size);
		
		std::array<char, 256> buffer;
		buffer.back() = '\n';
		strncpy(buffer.data(), m_openEventName.c_str(), buffer.size());
		
		if (ImGui::InputText("Open Event", buffer.data(), buffer.size()))
			m_openEventName = buffer.data();
	}
	
	nlohmann::json DoorEntity::Serialize() const
	{
		nlohmann::json json;
		
		json["transform"] = GetTransform().Serialize(Transform::Properties::Position | Transform::Properties::Rotation);
		json["open_event"] = m_openEventName;
		json["size"] = GetSizeX() * 2;
		
		return json;
	}
	
	std::unique_ptr<Entity> DoorEntity::Clone() const
	{
		return std::make_unique<DoorEntity>(*this);
	}
}
