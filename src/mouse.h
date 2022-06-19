#pragma once

#include <glm/vec2.hpp>

namespace TankGame
{
	enum class MouseButton : uint8_t
	{
		Left   = 1,
		Right  = 2,
		Middle = 4,
	};
	
	class Mouse
	{
	public:
		Mouse() = default;
		
		inline float GetOldX() const
		{ return m_oldX; }
		inline float GetOldY() const
		{ return m_oldY; }
		inline glm::vec2 GetOldPosition() const
		{ return { m_oldX, m_oldY }; }
		
		inline bool IsDown(MouseButton button) const
		{ return (m_buttonState & (uint8_t)button) != 0 && !isCaptured; }
		inline bool WasDown(MouseButton button) const
		{ return (m_preButtonState & (uint8_t)button) != 0 && !m_wasCaptured; }
		
		inline float GetDeltaScroll() const
		{ return scrollPos - m_oldScroll; }
		
		void OnFrameEnd()
		{
			m_oldX = pos.x;
			m_oldY = pos.y;
			m_oldScroll = scrollPos;
			m_preButtonState = m_buttonState;
			m_wasCaptured = isCaptured;
		}
		
		bool isCaptured = false;
		
		glm::vec2 pos;
		float scrollPos = 0;
		
		uint8_t m_buttonState = 0;
		
	private:
		float m_oldX = 0;
		float m_oldY = 0;
		
		float m_oldScroll = 0;
		
		bool m_wasCaptured = false;
		
		uint8_t m_preButtonState = 0;
	};
}
