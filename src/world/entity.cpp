#include "entity.h"
#include "gameworld.h"

#include <imgui.h>

namespace TankGame
{
	Rectangle Entity::GetBoundingRectangle() const
	{
		return GetBoundingCircle().GetBoundingRectangle();
	}
	
	Circle Entity::GetBoundingCircle() const
	{
		return GetTransform().GetBoundingCircle();
	}
	
	void Entity::Despawn()
	{
		m_world->Despawn(this);
	}
	
	void Entity::OnSpawned(class GameWorld& gameWorld)
	{
		m_world = &gameWorld;
	}
	
	nlohmann::json Entity::Serialize() const
	{
		nlohmann::json json;
		json["transform"] = m_transform.Serialize(Transform::Properties::All);
		return json;
	}
	
	std::unique_ptr<Entity> Entity::Clone() const
	{
		return nullptr;
	}
	
	void Entity::RenderProperties()
	{
		RenderTransformProperty();
	}
	
	const char* Entity::GetObjectName() const
	{
		return "Entity";
	}
	
	void Entity::RenderTransformProperty(Transform::Properties propertiesToShow)
	{
		if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
		{
			GetTransform().RenderProperties(propertiesToShow);
			ImGui::TreePop();
		}
	}
}
