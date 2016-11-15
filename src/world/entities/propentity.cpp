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
			throw std::runtime_error("");
		return *propClass;
	}
	
	PropEntity::PropEntity(const std::string& propClassName, bool isSolid)
	    : m_propClass(&GetPropClass(propClassName)), m_isSolid(isSolid),
	      m_aspectRatio(m_propClass->GetTextureHeight() / static_cast<float>(m_propClass->GetTextureWidth()))
	{
		SetSizeX(0.5f);
	}
	
	void PropEntity::Draw(SpriteRenderList& spriteRenderList) const
	{
		spriteRenderList.Add(GetTransform(), m_propClass->GetMaterial(), m_z);
	}
	
	void PropEntity::SetSizeX(float sizeX)
	{
		m_sizeX = sizeX;
		GetTransform().SetScale({ sizeX, sizeX * m_aspectRatio });
	}
	
	IntersectInfo PropEntity::GetIntersectInfo(const Circle& circle) const
	{
		if (!m_isSolid)
			return { };
		OrientedRectangle orientedRectangle = OrientedRectangle::FromTransformedNDC(GetTransform());
		return orientedRectangle.GetIntersectInfo(circle);
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
		
		return json;
	}
	
	void PropEntity::RenderProperties()
	{
		RenderTransformProperty(Transform::Properties::Position | Transform::Properties::Rotation);
		
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
