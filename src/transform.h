#pragma once

#include <glm/glm.hpp>
#include <json.hpp>
#include "circle.h"

namespace TankGame
{
	class Transform
	{
	public:
		enum class Properties
		{
			None = 0,
			Position = 1,
			Scale = 2,
			Rotation = 4,
			All = 7
		};
		
		inline void Rotate(float rotation)
		{ SetRotation(m_rotation + rotation); }
		inline void Translate(glm::vec2 translation)
		{ SetPosition(m_position + translation); }
		inline void Scale(glm::vec2 scale)
		{ SetScale(m_scale * scale); }
		
		inline float GetRotation() const
		{ return m_rotation; }
		inline glm::vec2 GetPosition() const
		{ return m_position; }
		inline glm::vec2 GetScale() const
		{ return m_scale; }
		
		Circle GetBoundingCircle() const;
		Circle GetInscribedCircle() const;
		
		void SetRotation(float rotation);
		void SetPosition(glm::vec2 position);
		void SetScale(glm::vec2 scale);
		
		void SetCenterOfRotation(glm::vec2 centerOfRotation);
		inline glm::vec2 GetCenterOfRotation() const
		{ return m_centerOfRotation; }
		
		const glm::mat3& GetMatrix() const;
		
		inline float GetCosRotation() const
		{ return m_cosRot; }
		inline float GetSinRotation() const
		{ return m_sinRot; }
		
		inline glm::vec2 GetForward() const
		{ return { m_sinRot, -m_cosRot }; }
		
		nlohmann::json Serialize(Properties propertiesToSerialize) const;
		
		bool RenderProperties(Properties propertiesToShow);
		
	private:
		float m_rotation = 0.0f;
		glm::vec2 m_centerOfRotation = { 0.0f, 0.0f };
		
		glm::vec2 m_position = { 0.0f, 0.0f };
		glm::vec2 m_scale = { 1.0f, 1.0f };
		
		float m_cosRot = 1;
		float m_sinRot = 0;
		
		mutable glm::mat3 m_matrix;
		mutable bool m_matrixOutOfDate = false;
	};
	
	inline Transform::Properties operator|(Transform::Properties a, Transform::Properties b)
	{ return static_cast<Transform::Properties>(static_cast<int>(a) | static_cast<int>(b)); }
	inline int operator&(Transform::Properties a, Transform::Properties b)
	{ return static_cast<int>(a) & static_cast<int>(b); }
}
