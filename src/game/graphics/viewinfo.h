#pragma once

#include <glm/glm.hpp>

#include "../rectangle.h"

namespace TankGame
{
	class ViewInfo
	{
	public:
		inline ViewInfo() { }
		ViewInfo(glm::vec2 focusPoint, float rotation, float viewWidth, float aspectRatio);
		
		explicit ViewInfo(const glm::mat3& viewMatrix, const glm::mat3* inverseViewMatrix);
		
		inline const glm::mat3& GetWorldViewMatrix() const
		{ return m_worldViewMatrix; }
		
		inline const glm::mat3& GetViewMatrix() const
		{ return m_viewMatrix; }
		inline const glm::mat3& GetInverseViewMatrix() const
		{ return m_inverseViewMatrix; }
		inline const Rectangle& GetViewRectangle() const
		{ return m_viewRectangle; }
		
		inline const std::array<glm::vec2, 4>& GetWorldViewCorners() const
		{ return m_worldViewCorners; };
		
		bool Visible(const class Circle& cirlce) const;
		
		//Accepts values in screen space where x and y are in the range [0, 1)
		glm::vec2 ScreenToWorld(glm::vec2 screenCoord) const;
		
		//Returns values in screen space where x and y are in the range [0, 1)
		glm::vec2 WorldToScreen(glm::vec2 worldPosition) const;
		
	private:
		void InitializeViewRectangle();
		
		glm::mat3 m_worldViewMatrix;
		glm::mat3 m_viewMatrix;
		glm::mat3 m_inverseViewMatrix;
		Rectangle m_viewRectangle;
		
		std::array<glm::vec2, 4> m_worldViewCorners;
		std::array<glm::vec2, 4> m_worldViewNormals;
	};
}
