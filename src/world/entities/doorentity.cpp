#include "doorentity.h"
#include "../gameworld.h"
#include "../../audio/soundeffectplayer.h"

#include <imgui.h>
#include <algorithm>

namespace TankGame
{
	static SoundEffectPlayer soundEffectPlayer{ "DoorOpen" };
	
	DoorEntity::DoorEntity(const std::string* openEvents, size_t numOpenEvents)
	    : PropEntity("Door", true), m_openEvents(numOpenEvents)
	{
		SetSizeX(0.5f);
		
		for (size_t i = 0; i < numOpenEvents; i++)
			m_openEvents[i] = OpenEvent(openEvents[i]);
	}
	
	void DoorEntity::OnSpawned(GameWorld& gameWorld)
	{
		if (gameWorld.GetWorldType() == GameWorld::Types::Game)
		{
			for (const OpenEvent& eventName : m_openEvents)
				gameWorld.ListenForEvent(eventName.m_name, *this);
		}
		
		PropEntity::OnSpawned(gameWorld);
	}
	
	void DoorEntity::HandleEvent(const std::string& event, Entity* sender)
	{
		auto evPos = std::find_if(m_openEvents.begin(), m_openEvents.end(), [&] (const OpenEvent& ev)
		{
			return ev.m_name == event;
		});
		
		if (evPos != m_openEvents.end())
		{
			evPos->m_received = true;
			
			if (std::all_of(m_openEvents.begin(), m_openEvents.end(), [&] (const auto& ev) { return ev.m_received; }))
				Open();
		}
		
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
		
		std::array<char, 256> inputBuffer;
		inputBuffer.back() = '\n';
		
		if (ImGui::ListBoxHeader("Open Events", m_openEvents.size(), 4))
		{
			ImGui::PushID("OpenEvents");
			
			for (int i = 0; i < static_cast<int>(m_openEvents.size()); i++)
			{
				strncpy(inputBuffer.data(), m_openEvents[i].m_name.c_str(), inputBuffer.size());
				
				ImGui::PushID(static_cast<int>(i));
				
				if (ImGui::InputText("##OpenEvent", inputBuffer.data(), inputBuffer.size()))
					m_openEvents[i].m_name = inputBuffer.data();
				
				ImGui::SameLine();
				
				if (ImGui::Button("Remove"))
				{
					m_openEvents.erase(m_openEvents.begin() + i);
					i--;
				}
				
				ImGui::PopID();
			}
			
			if (ImGui::Button("Add"))
				m_openEvents.emplace_back("OpenEvent");
			
			ImGui::PopID();
			ImGui::ListBoxFooter();
		}
	}
	
	nlohmann::json DoorEntity::Serialize() const
	{
		nlohmann::json json;
		
		std::vector<std::string> openEventNames;
		for (size_t i = 0; i < m_openEvents.size(); i++)
			openEventNames.push_back(m_openEvents[i].m_name);
		
		json["transform"] = GetTransform().Serialize(Transform::Properties::Position | Transform::Properties::Rotation);
		json["open_event"] = openEventNames;
		json["size"] = GetSizeX() * 2;
		
		return json;
	}
	
	std::unique_ptr<Entity> DoorEntity::Clone() const
	{
		return std::make_unique<DoorEntity>(*this);
	}
}
