#pragma once

#include "graphics/worldrenderer.h"
#include "graphics/deferredrenderer.h"
#include "graphics/spritematerial.h"
#include "graphics/tilegridmaterial.h"
#include "graphics/shadowrenderer.h"
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
#include "utils/memory/stackobject.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

namespace TankGame
{
	class Window
	{
	public:
		Window(const ArgumentData& argumentData);
		
		void RunGame();
		
		void SetEnableVSync(bool enableVSync);
		
		void MakeFullscreen(int resX, int resY);
		void MakeWindowed();
		
		static constexpr int MIN_WIDTH = 600;
		static constexpr int MIN_HEIGHT = 400;
		
	private:
		static void ResizeCallback(GLFWwindow* glfwWindow, int newWidth, int newHeight);
		
		void LoadLevel(const std::string& name);
		void EditLevel(const std::string& name);
		
		void Initialize();
		void RunFrame(float dt);
		
		ArgumentData m_argumentData;
		
		GLFWwindow* m_window;
		
		int m_width, m_height;
		
		float m_gameTime = 0;
		
		float m_aspectRatio = 1;
		ViewInfo m_viewInfo;
		
		Keyboard m_keyboard;
		Mouse m_mouse;
		
		bool m_isFullscreen = false;
		
		bool m_initialized = false;
		
		StackObject<Editor> m_editor;
		
		Console m_console;
		
		StackObject<MenuManager> m_menuManager;
		
		StackObject<GameManager> m_gameManager;
		
		StackObject<ShadowRenderer> m_shadowRenderer;
		
		StackObject<DeferredRenderer> m_deferredRenderer;
	};
}
