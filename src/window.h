#pragma once

#include "graphics/worldrenderer.h"
#include "graphics/deferredrenderer.h"
#include "graphics/spritematerial.h"
#include "graphics/tilegridmaterial.h"
#include "graphics/shadowrenderer.h"
#include "graphics/frames.h"
#include "world/gameworld.h"
#include "ui/menu/menumanager.h"
#include "gamemanager.h"
#include "graphics/viewinfo.h"
#include "editor/editor.h"
#include "audio/ambienceplayer.h"
#include "keyboard.h"
#include "mouse.h"
#include "level.h"
#include "argumentdata.h"
#include "console.h"
#include "loadingscreen.h"
#include "asyncworklist.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>

namespace TankGame
{
	class Window
	{
	public:
		explicit Window(const ArgumentData& arguments);
		
		void RunGame();
		
		void MakeFullscreen(int resX, int resY);
		void MakeWindowed();
		
		static constexpr int MIN_WIDTH = 600;
		static constexpr int MIN_HEIGHT = 400;
		
	private:
		static void ResizeCallback(GLFWwindow* glfwWindow, int newWidth, int newHeight);
		
		void LoadLevel(const std::string& name);
		void EditLevel(const std::string& name);
		
		void Initialize();
		void InitializeConsole();
		void RunFrame(float dt);
		void SetIsCursorCaptured(bool shouldCapture);
		
		std::unique_ptr<LoadingScreen> m_loadingScreen;
		
		std::array<GLsync, MAX_QUEUED_FRAMES> m_frameFences;
		
		ArgumentData m_arguments;
		
		GLFWwindow* m_window;
		
		bool m_isCursorCaptured = false;
		
		bool m_isFocused = true;
		
		int m_width, m_height;
		
		float m_gameTime = 0;
		
		float m_aspectRatio = 1;
		ViewInfo m_viewInfo;
		
		Keyboard m_keyboard;
		Mouse m_mouse;
		
		bool m_isFullscreen = false;
		
		bool m_initialized = false;
		
		std::unique_ptr<Editor> m_editor;
		
		Console m_console;
		
		std::unique_ptr<MenuManager> m_menuManager;
		
		std::unique_ptr<GameManager> m_gameManager;
		
		std::unique_ptr<ShadowRenderer> m_shadowRenderer;
		
		std::unique_ptr<DeferredRenderer> m_deferredRenderer;
	};
}
