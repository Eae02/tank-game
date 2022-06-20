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
		friend class Window;
		
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
		
		inline uint8_t GetButtonState() const { return m_buttonState; }
		
		bool isCaptured = false;
		
		glm::vec2 pos;
		float scrollPos = 0;
		
	private:
		void OnFrameEnd() //Called by Window
		{
			m_oldX = pos.x;
			m_oldY = pos.y;
			m_oldScroll = scrollPos;
			m_preButtonState = m_buttonState;
			m_wasCaptured = isCaptured;
		}
		
		uint8_t m_buttonState = 0;
		
		float m_oldX = 0;
		float m_oldY = 0;
		
		float m_oldScroll = 0;
		
		bool m_wasCaptured = false;
		
		uint8_t m_preButtonState = 0;
	};
}
