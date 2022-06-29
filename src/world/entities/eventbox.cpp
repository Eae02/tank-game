#include "eventbox.h"
#include "playerentity.h"
#include "../gameworld.h"
#include "../../updateinfo.h"
#include "../../graphics/ui/uirenderer.h"
#include "../../orientedrectangle.h"
#include "../../utils/imgui.h"
#include "../../lua/luavm.h"

#include <nlohmann/json.hpp>
#include <imgui.h>

namespace TankGame
{
	void EventBox::Update(const UpdateInfo& updateInfo)
	{
		if (m_player == nullptr)
			return;
		
		OrientedRectangle rectangle = OrientedRectangle::FromTransformedNDC(GetTransform());
		
		bool isPlayerInBox = rectangle.GetIntersectInfo(m_player->GetHitCircle()).m_intersects;
		
		if (isPlayerInBox && !m_wasPlayerInBox && !m_playerEnterScript.empty())
			Lua::DoString(m_playerEnterScript, GetGameWorld()->GetLuaSandbox());
		else if (!isPlayerInBox && m_wasPlayerInBox && !m_playerLeaveScript.empty())
			Lua::DoString(m_playerLeaveScript, GetGameWorld()->GetLuaSandbox());
		
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
		RenderBaseProperties();
		
		RenderScriptEditor("Player Enter Script", m_playerEnterScript);
		
		RenderScriptEditor("Player Leave Script", m_playerLeaveScript);
	}
	
	const char* EventBox::GetObjectName() const
	{ return "Event Box"; }
	const char* EventBox::GetSerializeClassName() const
	{ return "EventBox"; }
	
	nlohmann::json EventBox::Serialize() const
	{
		nlohmann::json json = Entity::Serialize();
		
		if (!m_playerEnterScript.empty())
			json["player_enter"] = m_playerEnterScript;
		if (!m_playerLeaveScript.empty())
			json["player_leave"] = m_playerLeaveScript;
		
		return json;
	}
	
	std::unique_ptr<Entity> EventBox::Clone() const
	{
		return std::make_unique<EventBox>(*this);
	}
}
