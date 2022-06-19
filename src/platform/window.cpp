#include "window.h"

namespace TankGame
{
	void Window::SetCursorCapture(bool shouldCapture)
	{
		if (shouldCapture == m_isCursorCaptured || arguments.m_noCursorGrab)
			return;
		m_isCursorCaptured = shouldCapture;
		Platform_CursorCapturedChanged();
	}
}
