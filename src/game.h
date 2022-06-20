#pragma once

#include "gamemanager.h"
#include "platform/common.h"
#include "editor/editor.h"
#include "ui/menu/menumanager.h"
#include "graphics/deferredrenderer.h"
#include "loadingscreen.h"
#include "console.h"

namespace TankGame
{
	struct Game
	{
		double lastFrameStartTime = 0;
		float gameTime = 0;
		
		float aspectRatio = 1;
		ViewInfo m_viewInfo;
		
		std::unique_ptr<Editor> editor;
		
		DeferredRenderer deferredRenderer;
		ShadowRenderer shadowRenderer;
		std::unique_ptr<MenuManager> menuManager;
		std::unique_ptr<GameManager> gameManager;
		Console console;
		
		std::unique_ptr<LoadingScreen> loadingScreen;
		
		int windowWidth;
		int windowHeight;
		
		VideoModes videoModes;
		
		void Initialize(class Window& window);
		
		void LoadLevel(const std::string& name);
		void EditLevel(const std::string& name);
		
		void RunFrame(class Window& window);
	};
	
	void RunGame(const struct ArgumentData& arguments, const VideoModes& videoModes);
}
