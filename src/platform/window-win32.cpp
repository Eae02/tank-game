#ifdef _WIN32
#include "window.h"

#define GLFW_EXPOSE_NATIVE_WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace TankGame
{
	void Window::Platform_CursorCapturedChanged()
	{
		if (!m_isCursorCaptured)
			ClipCursor(nullptr);
	}
	
	void Window::Platform_UpdateCursorCapture()
	{
		HWND hwnd = glfwGetWin32Window(m_window);
		
		POINT clientUL, clientLR;
		
		RECT clientRectangle;
		GetClientRect(hwnd, &clientRectangle); 
		clientUL.x = clientRectangle.left; 
		clientUL.y = clientRectangle.top; 
		
		clientLR.x = clientRectangle.right; 
		clientLR.y = clientRectangle.bottom; 
		ClientToScreen(hwnd, &clientUL); 
		ClientToScreen(hwnd, &clientLR);
		
		SetRect(&clientRectangle, clientUL.x, clientUL.y, clientLR.x, clientLR.y); 
		
		ClipCursor(&clientRectangle);
	}
}

#endif
