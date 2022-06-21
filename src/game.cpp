#include "game.h"
#include "argumentdata.h"
#include "platform/window.h"

#include "console.h"
#include "consolecmd.h"
#include "graphics/gl/functions.h"
#include "settings.h"
#include "progress.h"
#include "graphics/quadmesh.h"
#include "graphics/ui/font.h"
#include "world/lights/pointlightentity.h"
#include "world/particles/systems/smokeparticlesystem.h"
#include "world/entities/playerentity.h"
#include "world/entities/checkpointentity.h"
#include "audio/almanager.h"
#include "utils/ioutils.h"
#include "utils/mathutils.h"
#include "utils/utils.h"
#include "ui/levelslist.h"
#include "graphics/frames.h"
#include "platform/common.h"
#include "exceptions/fatalexception.h"
#include "world/props/propsmanager.h"
#include "world/serialization/serializeworld.h"
#include "imguiinterface.h"
#include "levelmenuinfo.h"
#include "loadingscreen.h"
#include "graphics/tilegridmaterial.h"

#include <string>
#include <array>

namespace TankGame
{
	//Defined in utils.cpp
	void OnClose();
	
	std::string glVendorName;
	
	void RunGame(const ArgumentData& arguments, const VideoModes& videoModes)
	{
		std::unique_ptr<Window> windowUP = std::make_unique<Window>(arguments);
		
		VertexInputState::InitEmpty();
		
		BufferAllocator::SetInstance(std::make_unique<BufferAllocator>());
		QuadMesh::SetInstance(std::make_unique<QuadMesh>());
		
		PropsManager::SetInstance(std::make_unique<PropsManager>());
		
		UIRenderer::SetInstance(std::make_unique<UIRenderer>());
		
		std::shared_ptr<Game> game = std::make_shared<Game>();
		
		game->videoModes = videoModes;
		game->loadingScreen = std::make_unique<LoadingScreen>();
		game->loadingScreen->Initialize();
		
		AddConsoleCommands(game->console, game);
		
		windowUP->uninitialize = [game] (Window& window)
		{
			TileGridMaterial::SetInstance(nullptr);
			UIRenderer::SetInstance(nullptr);
			Font::DestroyFonts();
			OnClose();
			PropsManager::SetInstance(nullptr);
			QuadMesh::SetInstance(nullptr);
			BufferAllocator::SetInstance(nullptr);
			ImGuiInterface::Close();
		};
		
		windowUP->updateCallback = [game] (Window& window)
		{
			bool shouldCaptureCursor = false;
			
			if (game->loadingScreen != nullptr)
			{
				game->loadingScreen->RunFrame();
				
				if (game->loadingScreen->IsLoadingDone())
				{
					game->loadingScreen = nullptr;
					
					double beforeInitialize = GetTime();
					game->Initialize(window);
					game->lastFrameStartTime = GetTime();
					
					GetLogStream() << "Initializing took " << (game->lastFrameStartTime - beforeInitialize) << "s\n";
				}
			}
			else if (game->gameManager->GetLevel() != nullptr && !game->gameManager->IsPaused() && window.isFocused)
			{
				shouldCaptureCursor = true;
			}
			
			window.SetCursorCapture(shouldCaptureCursor);
			
			if (game->loadingScreen == nullptr)
				game->RunFrame(window);
		};
		
		windowUP->resizeCallback = [game] (Window&, int newWidth, int newHeight)
		{
			game->windowWidth = newWidth;
			game->windowHeight = newHeight;
			
			glViewport(0, 0, newWidth, newHeight);
			Framebuffer::SetDefaultViewport(0, 0, newWidth, newHeight);
			
			game->aspectRatio = static_cast<float>(newWidth) / static_cast<float>(newHeight);
			
			if (game->loadingScreen != nullptr)
				game->loadingScreen->SetWindowSize(newWidth, newHeight);
			
			UIRenderer::SetSingletonWindowDimensions(newWidth, newHeight);
			
			if (game->menuManager != nullptr)
				game->menuManager->OnResize(newWidth, newHeight);
			
			game->deferredRenderer.CreateFramebuffer(newWidth, newHeight);
			game->shadowRenderer.OnResize(newWidth, newHeight);
			
			if (game->gameManager != nullptr)
				game->gameManager->OnResize(newWidth, newHeight);
			
			if (game->editor != nullptr)
				game->editor->OnResize(newWidth, newHeight);
		};
		
		Window::RunGameLoop(std::move(windowUP));
	}
	
	void Game::LoadLevel(const std::string& name)
	{
		gameManager->SetLevel(CommandCallbackLevelFromName(name, GameWorld::Types::Game));
		
		menuManager->Hide();
		if (editor != nullptr)
			editor->Close();
		
		Settings::instance.SetLastPlayedLevel(name);
		
		gameTime = 0;
	}
	
	void Game::EditLevel(const std::string& name)
	{
		menuManager->Hide();
		
		if (editor == nullptr)
		{
			editor = std::make_unique<Editor>(*gameManager);
			editor->OnResize(windowWidth, windowHeight);
		}
		
		editor->LoadLevel(name);
		gameManager->ExitLevel();
	}
	
	void Game::Initialize(Window& window)
	{
		LevelsList::LoadLevelMenuInfos();
		
		menuManager = std::make_unique<MenuManager>();
		gameManager = std::make_unique<GameManager>(deferredRenderer);
		
		menuManager->OnResize(window.width, window.height);
		gameManager->OnResize(window.width, window.height);
		
		menuManager->SetQuitCallback([wnd=&window] { wnd->Close(); });
		
		menuManager->SetLoadLevelCallback([this] (const std::string& name, int checkpoint)
		{
			LoadLevel(name);
			gameManager->GetLevel()->GetGameWorld().SetProgressLevelName(name);
			gameManager->GetLevel()->TryJumpToCheckpoint(checkpoint);
		});
		
		menuManager->SetSettingsApplyCallback([wnd=&window] (bool fullscreen, int resX, int resY)
		{
			if (fullscreen)
				wnd->MakeFullscreen(resX, resY);
			else
				wnd->MakeWindowed();
		});
		
		gameManager->SetQuitCallback([this]
		{
			if (gameManager->IsTesting())
				editor->StopTest();
			else
				menuManager->ShowMainMenu();
		});
		
		menuManager->ShowMainMenu();
		
		ImGuiInterface::Init();
	}
	
	void Game::RunFrame(Window& window)
	{
		frameBeginTime = GetTime();
		float dt = frameBeginTime - lastFrameStartTime;
		lastFrameStartTime = frameBeginTime;
		
		bool isEditorOpen = editor != nullptr && editor->IsOpen();
		
		if (gameManager->GetLevel() == nullptr)
			gameTime = 0;
		else if (!gameManager->IsPaused())
			gameTime += dt;
		
		UpdateInfo updateInfo(dt, gameTime, window.keyboard, window.mouse, m_viewInfo, window.width, window.height, isEditorOpen);
		updateInfo.m_videoModes = &videoModes;
		ImGuiInterface::NewFrame(updateInfo, dt);
		
		window.keyboard.isCaptured = ImGuiInterface::IsKeyboardCaptured();
		window.mouse.isCaptured = ImGuiInterface::IsMouseCaptured();
		
		if (menuManager->Visible())
			menuManager->Update(updateInfo);
		
		if (isEditorOpen)
			editor->Update(updateInfo);
		
		double gameUpdateBegin = window.arguments.m_profiling ? GetTime() : 0;
		gameManager->Update(updateInfo);
		double gameUpdateTime = window.arguments.m_profiling ? (GetTime() - gameUpdateBegin) : 0;
		
		//Shows the console when tilde is pressed
		if (window.keyboard.IsDown(Key::GraveAccent) && !window.keyboard.WasDown(Key::GraveAccent))
		{
			console.Show();
			if (gameManager->GetLevel() != nullptr)
				gameManager->Pause();
		}
		
		const WorldRenderer* worldRenderer = nullptr;
		if (isEditorOpen)
		{
			worldRenderer = &editor->GetRenderer();
			m_viewInfo = editor->GetViewInfo(aspectRatio);
		}
		else if (gameManager->GetLevel() != nullptr)
		{
			worldRenderer = &gameManager->GetRenderer();
			m_viewInfo = gameManager->GetLevel()->GetGameWorld().GetViewInfo(aspectRatio);
		}
		
		if (worldRenderer != nullptr)
			worldRenderer->UpdateResolution(window.width, window.height);
		
		if (deferredRenderer.FramebufferOutOfDate())
			deferredRenderer.CreateFramebuffer(window.width, window.height);
		
		if (worldRenderer != nullptr)
		{
			AudioSource::SetViewInfo(m_viewInfo);
			
			worldRenderer->Prepare(m_viewInfo, gameTime);
			worldRenderer->DrawShadowMaps(shadowRenderer, m_viewInfo);
			deferredRenderer.Draw(*worldRenderer, m_viewInfo);
			
			glEnable(GL_BLEND);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
			
			if (isEditorOpen)
				editor->DrawUI(m_viewInfo);
			gameManager->DrawUI();
			
			glDisable(GL_BLEND);
		}
		
		if (menuManager->Visible())
			menuManager->Draw(deferredRenderer, shadowRenderer, gameTime);
		
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		
		console.Render();
		
		ImGuiInterface::EndFrame();
		
		if (window.arguments.m_profiling)
		{
			std::ostringstream profileTextStream;
			
			profileTextStream
				<< "FPS: " << static_cast<int>(1.0f / dt)
				<< ", Frame Time: " << std::setprecision(3) << (dt * 1000) << "ms"
				<< ", Game Update Time: " << std::setprecision(3) << (gameUpdateTime * 1000) << "ms\n";
			
			profileTextStream << "SM Updates: " << shadowRenderer.lastFrameShadowMapUpdates << "\n";
			
			if (gameManager->GetLevel() != nullptr)
			{
				auto& particlesManager = gameManager->GetLevel()->GetGameWorld().GetParticlesManager();
				particlesManager.measureUpdateTime = true;
				
				profileTextStream << "Particle Update: " << std::setprecision(3) << (particlesManager.LastUpdateTime() * 1000.0) << "ms\n";
				profileTextStream << "Particle Draw (CPU): " << std::setprecision(3) << (deferredRenderer.GetParticleRenderer().LastDrawCPUTime() * 1000.0) << "ms\n";
				
				profileTextStream << "Lights: " << gameManager->GetRenderer().GetRenderedLights() << "\n";
				profileTextStream << "Particles: " << particlesManager.GetParticleCount() << "\n";
				profileTextStream << "Drawn Particles: " << deferredRenderer.GetParticleRenderer().GetNumRenderedParticles() << "\n";
			}
			
			Rectangle viewRect(0, 0, window.width, window.height);
			viewRect.Inflate(-5);
			
			std::vector<std::string> profInfoLines = Split(profileTextStream.str(), "\n");
			
			for (const std::string& line : profInfoLines)
			{
				UIRenderer::GetInstance().DrawString(Font::GetNamedFont(FontNames::StandardUI), line,
			                                      	 viewRect, Alignment::Left, Alignment::Top, glm::vec4(1.0f));
				viewRect.y -= 20;
			}
		}
		
		glDisable(GL_BLEND);
	}
}
