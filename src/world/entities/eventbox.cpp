#include "eventbox.h"
#include "playerentity.h"
#include "../gameworld.h"
#include "../../updateinfo.h"
#include "../../graphics/ui/uirenderer.h"
#include "../../orientedrectangle.h"

#include <imgui.h>

namespace TankGame
{
	void EventBox::Update(const UpdateInfo& updateInfo)
	{
		if (m_player == nullptr)
			return;
		
		OrientedRectangle rectangle = OrientedRectangle::FromTransformedNDC(GetTransform());
		
		bool isPlayerInBox = rectangle.GetIntersectInfo(m_player->GetHitCircle()).m_intersects;
		
		if (isPlayerInBox && !m_wasPlayerInBox && !m_playerEnterEvent.empty())
			GetGameWorld()->SendEvent(m_playerEnterEvent, this);
		else if (!isPlayerInBox && m_wasPlayerInBox && !m_playerLeaveEvent.empty())
			GetGameWorld()->SendEvent(m_playerLeaveEvent, this);
		
		m_wasPlayerInBox = isPlayerInBox;
	}
	
	void EventBox::OnSpawned(GameWorld& gameWorld)
	{
		m_player = dynamic_cast<const PlayerEntity*>(gameWorld.GetEntityByName("player"));
		Entity::OnSpawned(gameWorld);
	}
	
	void EventBox::DrawEditorUI(UIRenderer& uiRenderer, const glm::mat3& uiViewMatrix) const
	{
		glm::mat3 transform = uiViewMatrix * GetTransform().GetMatrix();
		
		uiRenderer.DrawQuad(transform, glm::vec4(1, 0, 0, 0.2f));
		uiRenderer.DrawQuadBorder(transform, glm::vec4(1, 0, 0, 1));
	}
	
	void EventBox::RenderProperties()
	{
		RenderTransformProperty();
		
		std::array<char, 256> buffer;
		buffer.back() = '\0';
		
		strncpy(buffer.data(), m_playerEnterEvent.c_str(), buffer.size() - 1);
		if (ImGui::InputText("Player Enter Event", buffer.data(), buffer.size()))
			m_playerEnterEvent = buffer.data();
		
		strncpy(buffer.data(), m_playerLeaveEvent.c_str(), buffer.size() - 1);
		if (ImGui::InputText("Player Leave Event", buffer.data(), buffer.size()))
			m_playerLeaveEvent = buffer.data();
	}
	
	const char* EventBox::GetObjectName() const
	{ return "Event Box"; }
	const char* EventBox::GetSerializeClassName() const
	{ return "EventBox"; }
	
	nlohmann::json EventBox::Serialize() const
	{
		nlohmann::json json = Entity::Serialize();
		
		if (!m_playerEnterEvent.empty())
			json["player_enter"] = m_playerEnterEvent;
		if (!m_playerLeaveEvent.empty())
			json["player_leave"] = m_playerLeaveEvent;
		
		return json;
	}
	
	std::unique_ptr<Entity> EventBox::Clone() const
	{
		return std::make_unique<EventBox>(*this);
	}
}
