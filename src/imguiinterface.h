#pragma once

#include "graphics/gl/functions.h"
#include <imgui.h>
#include <GLFW/glfw3.h>

namespace TankGame
{
	namespace ImGuiInterface
	{
		void Init(GLFWwindow* window);
		void NewFrame(GLFWwindow* window, float dt);
		void Close();
		
		bool IsMouseCaptured();
		bool IsKeyboardCaptured();
		
		void HandleKeyEvent(int key, int action);
		void HandleCharEvent(unsigned int c);
		void HandleScrollEvent(double yOffset);
	}
}
