#ifdef __EMSCRIPTEN__
#include "window.h"
#include "common.h"
#include "../graphics/gl/functions.h"
#include "../graphics/frames.h"
#include "../settings.h"
#include "../utils/utils.h"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <EGL/egl.h>
#include <imgui.h>

#include <unordered_map>
#include <unordered_set>

namespace TankGame
{
	static const std::unordered_map<std::string_view, Key> keyCodeMap = 
	{
		{ "Digit1", Key::D1 },
		{ "Digit2", Key::D2 },
		{ "KeyA", Key::A },
		{ "KeyB", Key::B },
		{ "KeyC", Key::C },
		{ "KeyD", Key::D },
		{ "KeyE", Key::E },
		{ "KeyF", Key::F },
		{ "KeyG", Key::G },
		{ "KeyH", Key::H },
		{ "KeyI", Key::I },
		{ "KeyJ", Key::J },
		{ "KeyK", Key::K },
		{ "KeyL", Key::L },
		{ "KeyM", Key::M },
		{ "KeyN", Key::N },
		{ "KeyO", Key::O },
		{ "KeyP", Key::P },
		{ "KeyQ", Key::Q },
		{ "KeyR", Key::R },
		{ "KeyS", Key::S },
		{ "KeyT", Key::T },
		{ "Key", Key::U },
		{ "KeyV", Key::V },
		{ "KeyW", Key::W },
		{ "KeyX", Key::X },
		{ "KeyY", Key::Y },
		{ "KeyZ", Key::Z },
		{ "F1", Key::F1 },
		{ "F2", Key::F2 },
		{ "F3", Key::F3 },
		{ "F4", Key::F4 },
		{ "F5", Key::F5 },
		{ "ShiftLeft", Key::LShift },
		{ "ShiftRight", Key::RShift },
		{ "ControlLeft", Key::LControl },
		{ "ControlRight", Key::RControl },
		{ "AltLeft", Key::LAlt },
		{ "AltRight", Key::RAlt },
		{ "Escape", Key::Escape },
		{ "Enter", Key::Enter },
		{ "Space", Key::Space },
		{ "Tab", Key::Tab },
		{ "Backspace", Key::Backspace },
		{ "ArrowLeft", Key::ArrowLeft },
		{ "ArrowUp", Key::ArrowUp },
		{ "ArrowRight", Key::ArrowRight },
		{ "ArrowDown", Key::ArrowDown },
		{ "Backquote", Key::GraveAccent },
		{ "PageUp", Key::PageUp },
		{ "PageDown", Key::PageDown },
		{ "Home", Key::Home },
		{ "End", Key::End },
		{ "Delete", Key::Delete }
	};
	
	static const std::unordered_set<std::string_view> nonTextKeys =
	{
		"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16",
		"F17", "F18", "F19", "F20", "F21", "F22", "F23", "Tab", "Backspace", "ArrowLeft", "ArrowUp", "ArrowRight",
		"ArrowDown", "PageUp", "PageDown", "Home", "End", "Delete", "CapsLock", "AltGraph", "Enter"
	};
	
	static uint64_t keyState;
	static glm::vec2 mouseCursorPos;
	static uint8_t mouseButtonState;
	static double scrollX = 0;
	static double scrollY = 0;
	
	std::unique_ptr<Window> theWindow;
	
	static EGLDisplay eglDisplay;
	static EGLSurface eglSurface;
	static EGLContext eglContext;
	
	Window::Window(const ArgumentData& _arguments) : arguments(_arguments)
	{
		eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		
		eglInitialize(eglDisplay, nullptr, nullptr);
		
		EGLConfig eglConfig;
		int numEglConfigs;
		eglGetConfigs(eglDisplay, &eglConfig, 1, &numEglConfigs);
		
		EGLint surfaceAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE, EGL_NONE };
		eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, 0, surfaceAttribs);
		if (eglSurface == EGL_NO_SURFACE)
		{
			Panic("eglCreateWindowSurface failed: " + std::to_string(eglGetError()));
		}
		
		std::vector<EGLint> contextAttribs = { EGL_CONTEXT_CLIENT_VERSION, 3 };
		contextAttribs.push_back(EGL_NONE);
		contextAttribs.push_back(EGL_NONE);
		
		eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttribs.data());
		if (eglContext == EGL_NO_CONTEXT)
		{
			Panic("eglCreateContext failed: " + std::to_string(eglGetError()));
		}
		
		if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext))
		{
			Panic("eglMakeCurrent failed: " + std::to_string(eglGetError()));
		}
	}
	
	void Window::RunGameLoop(std::unique_ptr<Window> window)
	{
		theWindow = std::move(window);
		
		emscripten_set_keydown_callback(nullptr, nullptr, true,
			[] (int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
		{
			if (!keyEvent->repeat)
			{
				auto keyIt = keyCodeMap.find(keyEvent->code);
				if (keyIt != keyCodeMap.end())
					keyState |= KeyToBitmask(keyIt->second);
			}
			if (keyEvent->location == DOM_KEY_LOCATION_STANDARD && nonTextKeys.find(keyEvent->key) == nonTextKeys.end())
			{
				ImGui::GetIO().AddInputCharactersUTF8(keyEvent->key);
			}
			return EM_TRUE;
		});
		
		emscripten_set_keyup_callback(nullptr, nullptr, true,
			[] (int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData)
		{
			if (!keyEvent->repeat)
			{
				auto keyIt = keyCodeMap.find(keyEvent->code);
				if (keyIt != keyCodeMap.end())
					keyState &= ~KeyToBitmask(keyIt->second);
			}
			return EM_TRUE;
		});
		
		emscripten_set_mousedown_callback(nullptr, nullptr, true,
			[] (int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
		{
			if (mouseEvent->button >= 0 && mouseEvent->button <= 2)
				mouseButtonState |= 1 << mouseEvent->button;
			return EM_TRUE;
		});
		
		emscripten_set_mouseup_callback(nullptr, nullptr, true,
			[] (int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
		{
			if (mouseEvent->button >= 0 && mouseEvent->button <= 2)
				mouseButtonState &= ~(uint8_t)(1 << mouseEvent->button);
			return EM_TRUE;
		});
		
		emscripten_set_mousemove_callback(nullptr, nullptr, true,
			[] (int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
		{
			mouseCursorPos.x = mouseEvent->clientX;
			mouseCursorPos.y = mouseEvent->clientY;
			return EM_TRUE;
		});
		
		emscripten_set_wheel_callback(nullptr, nullptr, true,
			[] (int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData)
		{
			scrollY -= wheelEvent->deltaY;
			scrollX -= wheelEvent->deltaX;
			return EM_TRUE;
		});
		
		emscripten_set_main_loop([]
		{
			int width, height;
			eglQuerySurface(eglDisplay, eglSurface, EGL_WIDTH, &width);
			eglQuerySurface(eglDisplay, eglSurface, EGL_HEIGHT, &height);
			if (width != theWindow->width || height != theWindow->height)
			{
				theWindow->width = width;
				theWindow->height = height;
				theWindow->resizeCallback(*theWindow, width, height);
			}
			
			theWindow->mouse.pos = glm::vec2(mouseCursorPos.x, (float)height - mouseCursorPos.y);
			theWindow->mouse.scrollPos = glm::vec2(scrollX, scrollY);
			theWindow->mouse.m_buttonState = mouseButtonState;
			theWindow->keyboard.m_keyStateBitmask = keyState;
			
			theWindow->updateCallback(*theWindow);
			
			theWindow->keyboard.OnFrameEnd();
			theWindow->mouse.OnFrameEnd();
		}, 0, 0);
		
		EM_ASM( loadingComplete(); );
	}
	
	void Window::Platform_CursorCapturedChanged() { }
	void Window::Platform_UpdateCursorCapture() { }
	void Window::MakeFullscreen(int resX, int resY) { }
	void Window::MakeWindowed() { }
	void Window::Close() { }
	void Window::CenterWindow() { }
}

#endif
