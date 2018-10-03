#include "keyboard.h"

#include <algorithm>
#include <iterator>

namespace TankGame
{
	Keyboard::Keyboard()
	{
		std::fill(std::begin(m_keyStates), std::end(m_keyStates), false);
		std::fill(std::begin(m_preKeyStates), std::end(m_preKeyStates), false);
	}
	
	bool Keyboard::IsKeyDown(int key) const
	{
		return m_keyStates[key] && !m_isCaptured;
	}
	
	bool Keyboard::WasKeyDown(int key) const
	{
		return m_preKeyStates[key] && !m_wasCaptured;
	}
	
	void Keyboard::KeyEvent(int key, int action)
	{
		if (action == GLFW_PRESS)
			m_keyStates[key] = true;
		else if (action == GLFW_RELEASE)
			m_keyStates[key] = false;
	}
	
	void Keyboard::OnFrameEnd()
	{
		std::copy(std::begin(m_keyStates), std::end(m_keyStates), m_preKeyStates);
		m_wasCaptured = m_isCaptured;
	}
}
