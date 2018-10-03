#pragma once

#include <glm/vec2.hpp>

namespace TankGame
{
	class Mouse
	{
	public:
		Mouse();
		
		inline float GetX() const
		{ return m_x; }
		inline float GetY() const
		{ return m_y; }
		
		inline float GetOldX() const
		{ return m_oldX; }
		inline float GetOldY() const
		{ return m_oldY; }
		
		inline glm::vec2 GetPosition() const
		{ return { m_x, m_y }; }
		inline glm::vec2 GetOldPosition() const
		{ return { m_oldX, m_oldY }; }
		
		inline bool IsButtonPressed(int button) const
		{ return m_buttonStates[button] && !m_isCaptured; }
		inline bool WasButtonPressed(int button) const
		{ return m_oldButtonStates[button] && !m_wasCaptured; }
		
		void OnFrameEnd();
		void MoveEvent(float newX, float newY);
		
		void ButtonEvent(int button, int action);
		inline void ScrollEvent(float yOffset)
		{ m_scrollPos += yOffset; }
		
		inline float GetDeltaScroll() const
		{ return m_scrollPos - m_oldScroll; }
		
		void SetIsCaptured(bool isCaptured)
		{ m_isCaptured = isCaptured; }
		inline bool IsCaptured() const
		{ return m_isCaptured; }
		
	private:
		float m_x = 0;
		float m_y = 0;
		float m_oldX = 0;
		float m_oldY = 0;
		
		float m_scrollPos = 0;
		float m_oldScroll = 0;
		
		bool m_isCaptured = false;
		bool m_wasCaptured = false;
		
		bool m_buttonStates[7];
		bool m_oldButtonStates[7];
	};
}
