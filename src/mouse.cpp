#include "mouse.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <iterator>

namespace TankGame
{
	Mouse::Mouse()
	{
		std::fill(std::begin(m_buttonStates), std::end(m_buttonStates), false);
		std::fill(std::begin(m_oldButtonStates), std::end(m_oldButtonStates), false);
	}
	
	void Mouse::OnFrameEnd()
	{
		m_oldX = m_x;
		m_oldY = m_y;
		
		m_oldScroll = m_scrollPos;
		
		std::copy(std::begin(m_buttonStates), std::end(m_buttonStates), m_oldButtonStates);
		
		m_wasCaptured = m_isCaptured;
	}
	
	void Mouse::MoveEvent(float newX, float newY)
	{
		m_x = newX;
		m_y = newY;
	}
	
	void Mouse::ButtonEvent(int button, int action)
	{
		m_buttonStates[button] = action == GLFW_PRESS;
	}
}
