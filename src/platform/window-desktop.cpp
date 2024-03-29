#ifndef __EMSCRIPTEN__
#include "window.h"
#include "common.h"
#include "../graphics/gl/functions.h"
#include "../graphics/frames.h"
#include "../settings.h"
#include "../utils/utils.h"


#include <imgui.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace TankGame
{
	static std::optional<Key> TranslateGlfwKey(int key)
	{
		switch (key)
		{
		case GLFW_KEY_A: return Key::A;
		case GLFW_KEY_B: return Key::B;
		case GLFW_KEY_C: return Key::C;
		case GLFW_KEY_D: return Key::D;
		case GLFW_KEY_E: return Key::E;
		case GLFW_KEY_F: return Key::F;
		case GLFW_KEY_G: return Key::G;
		case GLFW_KEY_H: return Key::H;
		case GLFW_KEY_I: return Key::I;
		case GLFW_KEY_J: return Key::J;
		case GLFW_KEY_K: return Key::K;
		case GLFW_KEY_L: return Key::L;
		case GLFW_KEY_M: return Key::M;
		case GLFW_KEY_N: return Key::N;
		case GLFW_KEY_O: return Key::O;
		case GLFW_KEY_P: return Key::P;
		case GLFW_KEY_Q: return Key::Q;
		case GLFW_KEY_R: return Key::R;
		case GLFW_KEY_S: return Key::S;
		case GLFW_KEY_T: return Key::T;
		case GLFW_KEY_U: return Key::U;
		case GLFW_KEY_V: return Key::V;
		case GLFW_KEY_W: return Key::W;
		case GLFW_KEY_X: return Key::X;
		case GLFW_KEY_Y: return Key::Y;
		case GLFW_KEY_Z: return Key::Z;
		case GLFW_KEY_1: return Key::D1;
		case GLFW_KEY_2: return Key::D2;
		case GLFW_KEY_F1: return Key::F1;
		case GLFW_KEY_F2: return Key::F2;
		case GLFW_KEY_F3: return Key::F3;
		case GLFW_KEY_F4: return Key::F4;
		case GLFW_KEY_F5: return Key::F5;
		case GLFW_KEY_UP: return Key::ArrowUp;
		case GLFW_KEY_DOWN: return Key::ArrowDown;
		case GLFW_KEY_LEFT: return Key::ArrowLeft;
		case GLFW_KEY_RIGHT: return Key::ArrowRight;
		case GLFW_KEY_END: return Key::End;
		case GLFW_KEY_HOME: return Key::Home;
		case GLFW_KEY_ENTER: return Key::Enter;
		case GLFW_KEY_SPACE: return Key::Space;
		case GLFW_KEY_TAB: return Key::Tab;
		case GLFW_KEY_BACKSPACE: return Key::Backspace;
		case GLFW_KEY_DELETE: return Key::Delete;
		case GLFW_KEY_ESCAPE: return Key::Escape;
		case GLFW_KEY_PAGE_UP: return Key::PageUp;
		case GLFW_KEY_PAGE_DOWN: return Key::PageDown;
		case GLFW_KEY_GRAVE_ACCENT: return Key::GraveAccent;
		case GLFW_KEY_LEFT_SHIFT: return Key::LShift;
		case GLFW_KEY_RIGHT_SHIFT: return Key::RShift;
		case GLFW_KEY_LEFT_CONTROL: return Key::LControl;
		case GLFW_KEY_RIGHT_CONTROL: return Key::RControl;
		case GLFW_KEY_LEFT_ALT: return Key::LAlt;
		case GLFW_KEY_RIGHT_ALT: return Key::RAlt;
		case GLFW_KEY_LEFT_SUPER: return Key::LSuper;
		case GLFW_KEY_RIGHT_SUPER: return Key::RSuper;
		default: return {};
		}
	}
	
	void SetDefaultWindowSize(Window& window, int fullscreenResX, int fullscreenResY)
	{
		if (window.isFullscreen)
		{
			window.width = fullscreenResX;
			window.height = fullscreenResY;
		}
		else
		{
			const GLFWvidmode* defaultVideoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			window.width = static_cast<int>(defaultVideoMode->width * 0.8);
			window.height = static_cast<int>(defaultVideoMode->height * 0.8);
		}
	}
	
	Window::Window(const ArgumentData& _arguments) : arguments(_arguments)
	{
		glfwWindowHint(GLFW_VISIBLE, false);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		
#ifndef NDEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
#endif
		
		isFullscreen = Settings::instance.IsFullscreen();
		SetDefaultWindowSize(*this, Settings::instance.GetFullscreenResolution().x, Settings::instance.GetFullscreenResolution().y);
		
		GLFWmonitor* monitor = isFullscreen ? glfwGetPrimaryMonitor() : nullptr;
		m_window = glfwCreateWindow(width, height, "Tank Game", monitor, nullptr);
		
		glfwSetWindowSizeLimits(m_window, MIN_RES_X, MIN_RES_Y, GLFW_DONT_CARE, GLFW_DONT_CARE);
		
		glfwSetWindowUserPointer(m_window, this);
		glfwSetWindowSizeCallback(m_window, [] (GLFWwindow* window, int width, int height)
		{
			Window* self = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
			self->width = width;
			self->height = height;
			self->resizeCallback(*self, width, height);
		});
		
		glfwSetWindowFocusCallback(m_window, [] (GLFWwindow* window, int hasFocus)
		{
			Window* self = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
			self->isFocused = hasFocus == GLFW_TRUE;
			
			if (self->isFullscreen && !self->isFocused)
				glfwIconifyWindow(window);
		});
		
		glfwSetKeyCallback(m_window, [] (GLFWwindow* window, int glfwKey, int, int action, int)
		{
			if (std::optional<Key> key = TranslateGlfwKey(glfwKey))
			{
				uint64_t keyMask = KeyToBitmask(*key);
				Keyboard& keyboard = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))->keyboard;
				if (action == GLFW_PRESS)
					keyboard.m_keyStateBitmask |= keyMask;
				else if (action == GLFW_RELEASE)
					keyboard.m_keyStateBitmask &= ~keyMask;
			}
		});
		
		glfwSetScrollCallback(m_window, [] (GLFWwindow* window, double xOffset, double yOffset)
		{
			reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))->mouse.scrollPos += glm::vec2(xOffset, yOffset);
		});
		
		glfwSetCharCallback(m_window, [] (GLFWwindow*, unsigned int codePoint)
		{
			if (codePoint < UINT16_MAX)
				ImGui::GetIO().AddInputCharacter(static_cast<unsigned short>(codePoint));
		});
		
		glfwShowWindow(m_window);
		
		CenterWindow();
		
		glfwMakeContextCurrent(m_window);
	}
	
	void Window::Close()
	{
		glfwSetWindowShouldClose(m_window, true);
	}
	
	void Window::CenterWindow()
	{
		if (!isFullscreen)
		{
			const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwSetWindowPos(m_window, (videoMode->width - width) / 2, (videoMode->height - height) / 2);
		}
	}
	
	void Window::MakeFullscreen(int resX, int resY)
	{
		isFullscreen = true;
		glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0, resX, resY, GLFW_DONT_CARE);
	}
	
	void Window::MakeWindowed()
	{
		if (!isFullscreen)
			return;
		isFullscreen = false;
		Settings::instance.SetIsFullscreen(false);
		SetDefaultWindowSize(*this, -1, -1);
		
		glfwSetWindowMonitor(m_window, nullptr, 0, 0, width, height, GLFW_DONT_CARE);
		CenterWindow();
	}
	
	void Window::RunGameLoop(std::unique_ptr<Window> window)
	{
		glfwGetWindowSize(window->m_window, &window->width, &window->height);
		window->resizeCallback(*window, window->width, window->height);
		
		glfwSwapInterval(window->enableVSync ? 1 : 0);
		window->m_isVSyncEnabled = window->enableVSync;
		
		//The main game loop
		while (!glfwWindowShouldClose(window->m_window))
		{
			if (window->m_isVSyncEnabled != window->enableVSync)
			{
				window->m_isVSyncEnabled = window->enableVSync;
				glfwSwapInterval(window->m_isVSyncEnabled ? 1 : 0);
			}
			
			glfwPollEvents();
			
			double cursorX, cursorY;
			glfwGetCursorPos(window->m_window, &cursorX, &cursorY);
			window->mouse.pos = glm::vec2(cursorX, window->height - cursorY);
			
			window->mouse.m_buttonState = 
				(glfwGetMouseButton(window->m_window, GLFW_MOUSE_BUTTON_LEFT)   ? (int)MouseButton::Left : 0) |
				(glfwGetMouseButton(window->m_window, GLFW_MOUSE_BUTTON_RIGHT)  ? (int)MouseButton::Right : 0) |
				(glfwGetMouseButton(window->m_window, GLFW_MOUSE_BUTTON_MIDDLE) ? (int)MouseButton::Middle : 0);
			
			if (window->m_isCursorCaptured)
				window->Platform_UpdateCursorCapture();
			
			window->updateCallback(*window);
			
			window->keyboard.OnFrameEnd();
			window->mouse.OnFrameEnd();
			
			glfwSwapBuffers(window->m_window);
		}
		
		if (window->m_isCursorCaptured)
		{
			window->m_isCursorCaptured = false;
			window->Platform_CursorCapturedChanged();
		}
		
		window->uninitialize(*window);
	}
}

#endif
