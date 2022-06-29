#pragma once

#include "../keyboard.h"
#include "../mouse.h"
#include "../argumentdata.h"

#include <functional>

struct GLFWwindow;

namespace TankGame
{
	class Window
	{
	public:
		explicit Window(const ArgumentData& _arguments);
		
		Window(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) = delete;
		
		void SetCursorCapture(bool shouldCapture);
		
		void MakeFullscreen(int resx, int resy);
		void MakeWindowed();
		
		void Close();
		
		std::function<void(Window&)> updateCallback;
		std::function<void(Window&, int newWidth, int newHeight)> resizeCallback;
		std::function<void(Window&)> uninitialize;
		
		static void RunGameLoop(std::unique_ptr<Window> window);
		
		bool isCursorCaptured = false;
		bool isFullscreen = false;
		bool isFocused = true;
		bool enableVSync = true;
		
		int width = 0;
		int height = 0;
		
		Keyboard keyboard;
		Mouse mouse;
		
		ArgumentData arguments;
		
	private:
		void Platform_CursorCapturedChanged();
		void Platform_UpdateCursorCapture();
		
		void CenterWindow();
		
		GLFWwindow* m_window;
		
		bool m_isCursorCaptured = false;
		bool m_isVSyncEnabled = false;
	};
}
