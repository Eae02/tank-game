#pragma once

#include <GLFW/glfw3.h>

namespace TankGame
{
	class Keyboard
	{
	public:
		Keyboard();
		
		bool IsKeyDown(int key) const;
		bool WasKeyDown(int key) const;
		
		void KeyEvent(int key, int action);
		void OnFrameEnd();
		
		void SetIsCaptured(bool isCaptured)
		{ m_isCaptured = isCaptured; }
		inline bool IsCaptured() const
		{ return m_isCaptured; }
		
	private:
		bool m_keyStates[GLFW_KEY_LAST];
		bool m_preKeyStates[GLFW_KEY_LAST];
		
		bool m_isCaptured = false;
		bool m_wasCaptured = false;
	};
}
