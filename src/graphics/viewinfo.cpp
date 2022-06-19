#include "viewinfo.h"
#include "../circle.h"

#include <limits>

namespace TankGame
{
	ViewInfo::ViewInfo(glm::vec2 focusPoint, float rotation, float viewWidth, float aspectRatio)
	{
		if (aspectRatio < 1)
			viewWidth *= aspectRatio;
		
		float scaleReciprocal = viewWidth / 2.0f;
		float scale = 1.0f / scaleReciprocal;
		
		float cosR = std::cos(rotation);
		float sinR = std::sin(rotation);
		
		m_worldViewMatrix = glm::transpose(glm::mat3(
				 cosR, sinR, 0.0f,
				-sinR, cosR, 0.0f,
				0.0f, 0.0f,  1.0f
		)) * glm::transpose(glm::mat3(
				1.0f, 0.0f, -focusPoint.x,
				0.0f, 1.0f, -focusPoint.y,
				0.0f, 0.0f, 1.0f
		));
		
		m_viewMatrix = glm::transpose(glm::mat3(
				scale, 0.0f, 0.0f,
				0.0f, scale * aspectRatio, 0.0f,
				0.0f, 0.0f, 1.0f
		)) * m_worldViewMatrix;
		
		m_inverseViewMatrix = glm::transpose(glm::mat3(
			1.0f, 0.0f, focusPoint.x,
			0.0f, 1.0f, focusPoint.y,
			0.0f, 0.0f, 1.0f
		)) * glm::transpose(glm::mat3(
			cosR, -sinR, 0.0f,
			sinR,  cosR, 0.0f,
			0.0f, 0.0f,  1.0f
		)) * glm::transpose(glm::mat3(
			scaleReciprocal, 0.0f, 0.0f,
			0.0f, scaleReciprocal * (1.0f / aspectRatio), 0.0f,
			0.0f, 0.0f, 1.0f
		));
		
		InitializeViewRectangle();
	}
	
	ViewInfo::ViewInfo(const glm::mat3& viewMatrix, const glm::mat3* inverseViewMatrix)
	    : m_viewMatrix(viewMatrix),
	      m_inverseViewMatrix(inverseViewMatrix == nullptr ? glm::inverse(m_viewMatrix) : *inverseViewMatrix)
	{
		InitializeViewRectangle();
	}
	
	glm::vec2 ViewInfo::ScreenToWorld(glm::vec2 screenCoord) const
	{
		glm::vec3 ndc(screenCoord.x * 2 - 1, screenCoord.y * 2 - 1, 1);
		return glm::vec2(m_inverseViewMatrix * ndc);
	}
	
	glm::vec2 ViewInfo::WorldToScreen(glm::vec2 worldPosition) const
	{
		return glm::vec2(m_viewMatrix * glm::vec3(worldPosition, 1)) * 0.5f + glm::vec2(0.5f);
	}
	
	void ViewInfo::InitializeViewRectangle()
	{
		glm::vec2 ndcVertices[4] = { { -1, -1 }, { -1, 1 }, { 1, 1 }, { 1, -1 } };
		glm::vec2 ndcNormals[4] = { { -1, 0 }, { 0, 1 }, { 1, 0 }, { 0, -1 } };
		
		glm::vec2 minVertexWS(std::numeric_limits<float>::infinity());
		glm::vec2 maxVertexWS(-std::numeric_limits<float>::infinity());
		
		for (size_t i = 0; i < 4; i++)
		{
			m_worldViewCorners[i] = glm::vec2(m_inverseViewMatrix * glm::vec3(ndcVertices[i], 1));
			
			if (m_worldViewCorners[i].x < minVertexWS.x)
				minVertexWS.x = m_worldViewCorners[i].x;
			if (m_worldViewCorners[i].y < minVertexWS.y)
				minVertexWS.y = m_worldViewCorners[i].y;
			if (m_worldViewCorners[i].x > maxVertexWS.x)
				maxVertexWS.x = m_worldViewCorners[i].x;
			if (m_worldViewCorners[i].y > maxVertexWS.y)
				maxVertexWS.y = m_worldViewCorners[i].y;
			
			m_worldViewNormals[i] = glm::normalize(glm::vec2(m_inverseViewMatrix * glm::vec3(ndcNormals[i], 0)));
		}
		
		m_viewRectangle = Rectangle::FromMinMax(minVertexWS, maxVertexWS);
	}
	
	bool ViewInfo::Visible(const Circle& circle) const
	{
		for (int i = 0; i < 4; i++)
		{
			float distToLine = glm::dot(m_worldViewNormals[i], circle.GetCenter()) -
				glm::dot(m_worldViewNormals[i], m_worldViewCorners[i]) - circle.GetRadius();
			
			if (distToLine > 0)
				return false;
		}
		
		return true;
	}
}
