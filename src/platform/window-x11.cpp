#ifdef __linux__
#include "window.h"

#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace TankGame
{
	void Window::Platform_CursorCapturedChanged()
	{
		if (!m_isCursorCaptured)
		{
			XUngrabPointer(glfwGetX11Display(), CurrentTime);
		}
		else
		{
			XGrabPointer(glfwGetX11Display(), glfwGetX11Window(m_window), true,
				ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask | EnterWindowMask | LeaveWindowMask,
				GrabModeAsync, GrabModeAsync, glfwGetX11Window(m_window), None, CurrentTime);
		}
	}
	
	void Window::Platform_UpdateCursorCapture() { }
}

#endif
