#include "propentity.h"
#include "../props/propsmanager.h"
#include "../props/propclass.h"
#include "../../graphics/spriterenderlist.h"
#include "../../orientedrectangle.h"

#include <imgui.h>

namespace TankGame
{
	static const PropClass& GetPropClass(const std::string& name)
	{
		const PropClass* propClass = PropsManager::GetInstance().GetPropClassByName(name);
		if (propClass == nullptr)
			throw std::runtime_error("Prop class not found: ' " + name + "'.");
		return *propClass;
	}
	
	PropEntity::PropEntity(const std::string& propClassName, float sizeX, bool isSolid)
	    : m_propClass(&GetPropClass(propClassName)), m_isSolid(isSolid),
	      m_aspectRatio(m_propClass->GetTextureHeight() / static_cast<float>(m_propClass->GetTextureWidth()))
	{
		SetSizeX(sizeX);
	}
	
	void PropEntity::DrawTranslucent(SpriteRenderList& spriteRenderList) const
	{
		if (m_propClass->IsDecal())
			spriteRenderList.Add(GetTransform(), m_propClass->GetMaterial(), m_z);
	}
	
	void PropEntity::Draw(SpriteRenderList& spriteRenderList) const
	{
		if (!m_propClass->IsDecal())
			spriteRenderList.Add(GetTransform(), m_propClass->GetMaterial(), m_z);
	}
	
	void PropEntity::SetSizeX(float sizeX)
	{
		m_sizeX = sizeX;
		GetTransform().SetScale({ sizeX, sizeX * m_aspectRatio });
	}
	
	ColliderInfo PropEntity::GetColliderInfo() const
	{
		if (!m_isSolid)
			return { };
		return OrientedRectangle::FromTransformedNDC(GetTransform());
	}
	
	CollidableTypes PropEntity::GetCollidableType() const
	{
		return CollidableTypes::Object;
	}
	
	const char* PropEntity::GetSerializeClassName() const
	{
		return "Prop";
	}
	
	nlohmann::json PropEntity::Serialize() const
	{
		nlohmann::json json = Entity::Serialize();
		
		json["prop_class"] = m_propClass->GetName();
		json["width"] = m_sizeX;
		json["solid"] = m_isSolid;
		json["z"] = m_z;
		
		return json;
	}
	
	void PropEntity::RenderProperties()
	{
		RenderBaseProperties(Transform::Properties::Position | Transform::Properties::Rotation);
		
		if (PropsManager::GetInstance().RenderPropClassSeletor("Prop Class", &m_propClass))
		{
			m_aspectRatio = m_propClass->GetTextureHeight() / static_cast<float>(m_propClass->GetTextureWidth());
			SetSizeX(m_sizeX);
		}
		
		if (ImGui::InputFloat("Width", &m_sizeX))
			SetSizeX(m_sizeX);
		
		ImGui::Checkbox("Solid", &m_isSolid);
		ImGui::SliderFloat("Z", &m_z, 0, 1);
	}
	
	const char* PropEntity::GetObjectName() const
	{
		return "Prop";
	}
	
	std::unique_ptr<Entity> PropEntity::Clone() const
	{ return std::make_unique<PropEntity>(*this); }
}
