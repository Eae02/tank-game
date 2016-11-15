#include "transform.h"

#include <cmath>
#include <algorithm>
#include <imgui.h>

namespace TankGame
{
	const glm::mat3& Transform::GetMatrix() const
	{
		if (m_matrixOutOfDate)
		{
			glm::vec2 scaledCOR = m_scale * m_centerOfRotation;
			
			//We need to transpose because the mat3 constructor takes column major arguments.
			
			glm::mat3 R = glm::transpose(glm::mat3(
					m_cosRot, -m_sinRot, -scaledCOR.x * m_cosRot + scaledCOR.y * m_sinRot + scaledCOR.x,
					m_sinRot,  m_cosRot, -scaledCOR.x * m_sinRot - scaledCOR.y * m_cosRot + scaledCOR.y,
					0, 0, 1
			));
			
			glm::mat3 T = glm::transpose(glm::mat3(
					1, 0, m_position.x - scaledCOR.x,
					0, 1, m_position.y - scaledCOR.y,
					0, 0, 1
			));
			
			glm::mat3 S = glm::transpose(glm::mat3(
					m_scale.x, 0, 0,
					0, m_scale.y, 0,
					0, 0, 1
			));
			
			m_matrix = T * R * S;
			
			m_matrixOutOfDate = false;
		}
		
		return m_matrix;
	}
	
	void Transform::SetRotation(float rotation)
	{
		m_rotation = rotation;
		
		m_cosRot = std::cos(rotation);
		m_sinRot = std::sin(rotation);
		
		m_matrixOutOfDate = true;
	}
	
	void Transform::SetPosition(glm::vec2 position)
	{
		m_position = position;
		m_matrixOutOfDate = true;
	}
	
	void Transform::SetScale(glm::vec2 scale)
	{
		m_scale = scale;
		m_matrixOutOfDate = true;
	}
	
	void Transform::SetCenterOfRotation(glm::vec2 centerOfRotation)
	{
		m_centerOfRotation = centerOfRotation;
		m_matrixOutOfDate = true;
	}
	
	Circle Transform::GetBoundingCircle() const
	{
		glm::vec3 corner1 = GetMatrix() * glm::vec3(-1, -1, 1);
		glm::vec3 corner2 = GetMatrix() * glm::vec3( 1,  1, 1);
		
		glm::vec2 center = (glm::vec2(corner1) + glm::vec2(corner2)) / 2.0f;
		
		return Circle(center, glm::length(glm::vec2(corner1) - center));
	}
	
	nlohmann::json Transform::Serialize(Properties propertiesToSerialize) const
	{
		nlohmann::json json;
		
		if (propertiesToSerialize & Properties::Position && (std::abs(m_position.x) > 1E-6 || std::abs(m_position.y) > 1E-6))
			json["position"] = { m_position.x, m_position.y };
		if (propertiesToSerialize & Properties::Scale && (std::abs(m_scale.x - 1) > 1E-6 || std::abs(m_scale.y - 1) > 1E-6))
			json["scale"] = { m_scale.x, m_scale.y };
		if (propertiesToSerialize & Properties::Rotation)
		{
			if (std::abs(m_rotation) > 1E-6)
				json["rotation"] = glm::degrees(m_rotation);
			if (std::abs(m_centerOfRotation.x) > 1E-6 || std::abs(m_centerOfRotation.y) > 1E-6)
				json["rotation_center"] = { m_centerOfRotation.x, m_centerOfRotation.y };
		}
		
		return json;
	}
	
	void Transform::RenderProperties(Properties propertiesToShow)
	{
		if (propertiesToShow & Properties::Position)
		{
			if (ImGui::InputFloat2("Position", reinterpret_cast<float*>(&m_position)))
				m_matrixOutOfDate = true;
		}
		
		if (propertiesToShow & Properties::Scale)
		{
			if (ImGui::InputFloat2("Scale", reinterpret_cast<float*>(&m_scale)))
				m_matrixOutOfDate = true;
		}
		
		if (propertiesToShow & Properties::Rotation)
		{
			if (ImGui::SliderAngle("Rotation", &m_rotation))
				SetRotation(m_rotation);
		}
	}
}
