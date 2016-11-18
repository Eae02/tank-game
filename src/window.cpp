#include <GL/glew.h>
#include "window.h"

#include "settings.h"
#include "graphics/quadmesh.h"
#include "graphics/ui/font.h"
#include "world/lights/pointlightentity.h"
#include "world/particles/systems/smokeparticlesystem.h"
#include "world/entities/playerentity.h"
#include "world/pathfinder.h"
#include "audio/almanager.h"
#include "utils/ioutils.h"
#include "utils/mathutils.h"
#include "utils/utils.h"
#include "world/props/propsmanager.h"
#include "world/serialization/serializeworld.h"
#include "imguiinterface.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <thread>
#include <chrono>
#include <imgui.h>
#include <glm/gtc/color_space.hpp>

namespace TankGame
{
	//Defined in utils.cpp
	void OnClose();
	
	std::string glVendorName;
	
	constexpr int Window::MIN_WIDTH;
	constexpr int Window::MIN_HEIGHT;
	
	Window::Window(const ArgumentData& argumentData)
	    : m_argumentData(argumentData)
	{
		glfwWindowHint(GLFW_VISIBLE, false);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		
#ifndef NDEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
		
		m_width = Settings::GetInstance().GetResolution().x;
		m_height = Settings::GetInstance().GetResolution().y;
		m_isFullscreen = Settings::GetInstance().IsFullscreen();
		
		GLFWmonitor* monitor = m_isFullscreen ? glfwGetPrimaryMonitor() : nullptr;
		m_window = glfwCreateWindow(m_width, m_height, "Tank Game", monitor, nullptr);
		
		glfwSetWindowSizeLimits(m_window, MIN_WIDTH, MIN_HEIGHT, GLFW_DONT_CARE, GLFW_DONT_CARE);
		
		glfwSetWindowUserPointer(m_window, this);
		glfwSetWindowSizeCallback(m_window, &Window::ResizeCallback);
		
		glfwSetWindowFocusCallback(m_window, [] (GLFWwindow* window, int hasFocus)
		{
			if (reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))->m_isFullscreen && hasFocus == GLFW_FALSE)
				glfwIconifyWindow(window);
		});
		
		glfwSetKeyCallback(m_window, [] (GLFWwindow* window, int key, int, int action, int)
		{
			reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))->m_keyboard.KeyEvent(key, action);
			ImGuiInterface::HandleKeyEvent(key, action);
		});
		
		glfwSetCursorPosCallback(m_window, [] (GLFWwindow* glfwWindow, double x, double y)
		{
			Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
			window->m_mouse.MoveEvent(x, window->m_height - y);
		});
		
		glfwSetMouseButtonCallback(m_window, [] (GLFWwindow* window, int button, int action, int mods)
		{
			reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))->m_mouse.ButtonEvent(button, action);
		});
		
		glfwSetScrollCallback(m_window, [] (GLFWwindow* window, double, double yOffset)
		{
			ImGuiInterface::HandleScrollEvent(yOffset);
			reinterpret_cast<Window*>(glfwGetWindowUserPointer(window))->m_mouse.ScrollEvent(static_cast<float>(yOffset));
		});
		
		glfwSetCharCallback(m_window, [] (GLFWwindow*, unsigned int codePoint)
		{
			ImGuiInterface::HandleCharEvent(codePoint);
		});
		
		InitializeConsole();
	}
	
	void Window::LoadLevel(const std::string& name)
	{
		m_menuManager->Hide();
		if (!m_editor.IsNull())
			m_editor->Close();
		
		m_gameTime = 0;
		m_gameManager->SetLevel(Level::FromName(name));
	}
	
	void Window::EditLevel(const std::string& name)
	{
		m_menuManager->Hide();
		
		if (m_editor.IsNull())
		{
			m_editor.Construct(*m_gameManager);
			m_editor->OnResize(m_width, m_height);
		}
		
		m_editor->LoadLevel(name);
		m_gameManager->ExitLevel();
	}
	
	void Window::InitializeConsole()
	{
		m_console.AddCommand("heal", [&] (const std::string* argv, size_t argc)
		{
			if (m_gameManager->GetLevel() == nullptr)
				throw std::runtime_error("No level loaded.");
			
			PlayerEntity& player = m_gameManager->GetLevel()->GetPlayerEntity();
			
			if (argc == 0)
				player.SetHp(player.GetMaxHp());
			else
				player.SetHp(player.GetHp() + std::stof(argv[0]));
		});
		
		m_console.AddCommand("event", [&] (const std::string* argv, size_t argc)
		{
			if (m_gameManager->GetLevel() == nullptr)
				throw std::runtime_error("No level loaded.");
			m_gameManager->GetLevel()->GetGameWorld().SendEvent(argv[0], nullptr);
		}, 1);
		
		m_console.AddCommand("edit", [&] (const std::string* argv, size_t argc) { EditLevel(argv[0]); }, 1);
		
		m_console.AddCommand("level", [&] (const std::string* argv, size_t argc) { LoadLevel(argv[0]); }, 1);
		
		m_console.AddCommand("newlevel", [&] (const std::string* argv, size_t argc)
		{
			int width = std::stoi(argv[1]);
			int height = std::stoi(argv[2]);
			
			if (width < 10 || height < 10)
				throw std::runtime_error("Level size must be at least 10x10.");
			
			fs::path path = Level::GetLevelsPath() / argv[0];
			if (fs::exists(path))
				throw std::runtime_error("The level aready exists.");
			
			std::ofstream stream(path.string(), std::ios::binary);
			WriteEmptyWorld(argv[0], width, height, stream);
		}, 3);
		
		m_console.AddCommand("menu", [this] (const std::string* argv, size_t argc)
		{
			if (m_menuManager->Visible())
				return;
			
			m_menuManager->ShowMainMenu();
			
			m_gameManager->ExitLevel();
			if (!m_editor.IsNull())
				m_editor->Close();
		});
		
		m_console.AddCommand("glinfo", [] (const std::string* argv, size_t argc)
		{
			GetLogStream() << "GL Renderer: " << glGetString(GL_RENDERER) << "\nGL Vendor: " << glVendorName << "\n";
		});
	}
	
	void Window::ResizeCallback(GLFWwindow* glfwWindow, int newWidth, int newHeight)
	{
		Window* window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
		
		glViewport(0, 0, newWidth, newHeight);
		Framebuffer::SetDefaultViewport(0, 0, newWidth, newHeight);
		
		window->m_width = newWidth;
		window->m_height = newHeight;
		
		window->m_aspectRatio = static_cast<float>(newWidth) / newHeight;
		
		if (!window->m_loadingScreen.IsNull())
			window->m_loadingScreen->SetWindowSize(newWidth, newHeight);
		
		UIRenderer::SetSingletonWindowDimensions(newWidth, newHeight);
		
		if (window->m_initialized)
		{
			window->m_menuManager->OnResize(newWidth, newHeight);
			
			window->m_deferredRenderer->OnResize(newWidth, newHeight);
			window->m_shadowRenderer->OnResize(newWidth, newHeight);
			window->m_gameManager->OnResize(newWidth, newHeight);
			
			if (!window->m_editor.IsNull())
				window->m_editor->OnResize(newWidth, newHeight);
		}
		
		Settings::GetInstance().SetResolution({ newWidth, newHeight });
	}
	
#ifndef NDEBUG
	static void GLAPIENTRY OpenGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	                                             const GLchar* message, const void* userParam)
	{
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
			return;
		
		if (glVendorName == "NVIDIA Corporation")
		{
			if (id == 131204 || id == 131169)
				return;
		}
		
		if (glVendorName == "Intel Open Source Technology Center")
		{
			if (id == 5 || id == 56 || id == 57)
				return;
		}
		
		if (severity == GL_DEBUG_SEVERITY_HIGH)
			throw std::runtime_error(message);
		
		if (severity == GL_DEBUG_SEVERITY_LOW || GL_DEBUG_SEVERITY_MEDIUM)
			GetLogStream() << LOG_WARNING;
		
		GetLogStream() << "GL #" << id << ": " << message;
		if (message[strlen(message) - 1] != '\n')
			GetLogStream() << "\n";
	}
#endif
	
	void Window::Initialize()
	{
		m_menuManager.Construct();
		m_menuManager->SetQuitCallback([this] { glfwSetWindowShouldClose(m_window, true); });
		
		m_menuManager->SetSettingsApplyCallback([this] (bool fullscreen, int resX, int resY)
		{
			if (fullscreen)
				MakeFullscreen(resX, resY);
			else
				MakeWindowed();
		});
		
		m_shadowRenderer.Construct();
		
		m_deferredRenderer.Construct();
		m_gameManager.Construct(*m_deferredRenderer);
		
		m_gameManager->SetQuitCallback([this]
		{
			if (m_gameManager->IsTesting())
				m_editor->StopTest();
			else
				m_menuManager->ShowMainMenu();
		});
		
		m_menuManager->SetBackground("Level1_0");
		
		m_menuManager->ShowMainMenu();
		
		ImGuiInterface::Init(m_window);
		
		m_initialized = true;
	}
	
	void Window::RunFrame(float dt)
	{
		bool isEditorOpen = !m_editor.IsNull() && m_editor->IsOpen();
		
		float gameTime = 0;
		if (m_gameManager->GetLevel() != nullptr && !m_gameManager->IsPaused())
		{
			m_gameTime += dt;
			gameTime = m_gameTime;
		}
		
		UpdateInfo updateInfo(dt, gameTime, m_keyboard, m_mouse, m_viewInfo, m_width, m_height, isEditorOpen);
		ImGuiInterface::NewFrame(m_window, dt);
		
		m_keyboard.SetIsCaptured(ImGuiInterface::IsKeyboardCaptured());
		m_mouse.SetIsCaptured(ImGuiInterface::IsMouseCaptured());
		
		if (m_menuManager->Visible())
			m_menuManager->Update(updateInfo);
		
		if (isEditorOpen)
			m_editor->Update(updateInfo);
		
		m_gameManager->Update(updateInfo);
		
		//Shows the console when tilde is pressed
		if (m_keyboard.IsKeyDown(GLFW_KEY_GRAVE_ACCENT) && !m_keyboard.WasKeyDown(GLFW_KEY_GRAVE_ACCENT))
		{
			m_console.Show();
			if (m_gameManager->GetLevel() != nullptr)
				m_gameManager->Pause();
		}
		
		const WorldRenderer* worldRenderer = nullptr;
		if (isEditorOpen)
		{
			worldRenderer = &m_editor->GetRenderer();
			m_viewInfo = m_editor->GetViewInfo(m_aspectRatio);
		}
		else if (m_gameManager->GetLevel() != nullptr)
		{
			worldRenderer = &m_gameManager->GetRenderer();
			m_viewInfo = m_gameManager->GetLevel()->GetGameWorld().GetViewInfo(m_aspectRatio);
		}
		
		if (worldRenderer != nullptr)
		{
			AudioSource::SetViewInfo(m_viewInfo);
			
			worldRenderer->Prepare(m_viewInfo);
			worldRenderer->DrawShadowMaps(*m_shadowRenderer, m_viewInfo);
			m_deferredRenderer->Draw(*worldRenderer, m_viewInfo);
			
			glEnable(GL_BLEND);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
			
			if (isEditorOpen)
				m_editor->DrawUI(m_viewInfo);
			m_gameManager->DrawUI();
			
			glDisable(GL_BLEND);
		}
		
		if (m_menuManager->Visible())
			m_menuManager->Draw(*m_deferredRenderer);
		
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		
		m_console.Render();
		
		ImGui::Render();
		
		if (m_argumentData.m_profiling)
		{
			std::ostringstream profileTextStream;
			profileTextStream << "FPS: " << static_cast<int>(1.0f / dt);
			profileTextStream << ", Frame Time: " << std::setprecision(3) << (dt * 1000) << "ms";
			if (m_gameManager->GetLevel() != nullptr)
			{
				profileTextStream << ", Lights: " << m_gameManager->GetRenderer().GetRenderedLights();
				profileTextStream << ", Particles: " <<
						m_gameManager->GetLevel()->GetGameWorld().GetParticlesManager().GetParticleCount();
			}
			
			Rectangle viewRect(0, 0, m_width, m_height);
			viewRect.Inflate(-5);
			
			UIRenderer::GetInstance().DrawString(Font::GetNamedFont(FontNames::StandardUI), profileTextStream.str(),
			                                     viewRect, Alignment::Left, Alignment::Bottom, glm::vec4(1.0f));
		}
		
		glDisable(GL_BLEND);
	}
	
	void Window::RunGame()
	{
		glfwShowWindow(m_window);
		
		if (!m_isFullscreen)
		{
			const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwSetWindowPos(m_window, (videoMode->width - m_width) / 2, (videoMode->height - m_height) / 2);
		}
		
		glfwMakeContextCurrent(m_window);
		
		glewExperimental = GL_TRUE;
		
		GLenum glewStatus = glewInit();
		if (glewStatus != GLEW_OK)
		{
			throw std::runtime_error(std::string("Error initializing GLEW: ") +
			                         reinterpret_cast<const char*>(glewGetErrorString(glewStatus)));
		}
		
		glVendorName = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
		
#ifndef NDEBUG
		if (glDebugMessageCallback && glDebugMessageControl)
		{
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLMessageCallback, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
#endif
		
		glDepthFunc(GL_LEQUAL);
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		BufferAllocator::SetInstance(std::make_unique<BufferAllocator>());
		QuadMesh::SetInstance(std::make_unique<QuadMesh>());
		
		PropsManager::SetInstance(std::make_unique<PropsManager>());
		
		UIRenderer::SetInstance(std::make_unique<UIRenderer>());
		
		m_loadingScreen.Construct();
		m_loadingScreen->Initialize();
		
		int width, height;
		glfwGetWindowSize(m_window, &width, &height);
		ResizeCallback(m_window, width, height);
		
		double cursorX, cursorY;
		glfwGetCursorPos(m_window, &cursorX, &cursorY);
		m_mouse.MoveEvent(cursorX, height - cursorY);
		
		double lastFrameTime = glfwGetTime();
		
		bool isVSyncEnabled = true;
		glfwSwapInterval(1);
		
		//The main game loop
		while (!glfwWindowShouldClose(m_window))
		{
			double currentTime = glfwGetTime();
			float elapsedTime = static_cast<float>(currentTime - lastFrameTime);
			lastFrameTime = currentTime;
			
			if (isVSyncEnabled != Settings::GetInstance().EnableVSync())
			{
				isVSyncEnabled = Settings::GetInstance().EnableVSync();
				glfwSwapInterval(isVSyncEnabled ? 1 : 0);
			}
			
			glfwPollEvents();
			
			if (!m_loadingScreen.IsNull())
			{
				m_loadingScreen->RunFrame();
				
				if (m_loadingScreen->IsLoadingDone())
				{
					m_loadingScreen.Destroy();
					
					double beforeInitialize = glfwGetTime();
					
					Initialize();
					glfwGetWindowSize(m_window, &width, &height);
					ResizeCallback(m_window, width, height);
					
					GetLogStream() << "Initializing took " << (glfwGetTime() - beforeInitialize) << "s\n";
				}
			}
			
			if (m_loadingScreen.IsNull())
				RunFrame(elapsedTime);
			
			m_keyboard.OnFrameEnd();
			m_mouse.OnFrameEnd();
			
			glFinish();
			
			glfwSwapBuffers(m_window);
			
			//Caps the FPS to 60Hz in menu screens
			if (m_loadingScreen.IsNull() && m_menuManager->Visible())
			{
				const int MENU_FPS = 60;
				double frameTime = glfwGetTime() - currentTime;
				
				double sleepTime = (1.0f / static_cast<float>(MENU_FPS)) - frameTime;
				if (sleepTime > 0)
					std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
			}
		}
		
		m_menuManager.Destroy();
		m_editor.Destroy();
		m_shadowRenderer.Destroy();
		m_deferredRenderer.Destroy();
		m_gameManager.Destroy();
		
		TileGridMaterial::SetInstance(nullptr);
		UIRenderer::SetInstance(nullptr);
		
		Font::DestroyFonts();
		
		OnClose();
		
		PropsManager::SetInstance(nullptr);
		QuadMesh::SetInstance(nullptr);
		BufferAllocator::SetInstance(nullptr);
		
		ImGuiInterface::Close();
	}
	
	void Window::SetEnableVSync(bool enableVSync)
	{
		Settings::GetInstance().SetEnableVSync(enableVSync);
		glfwSwapInterval(enableVSync ? 1 : 0);
	}
	
	void Window::MakeFullscreen(int resX, int resY)
	{
		m_isFullscreen = true;
		Settings::GetInstance().SetIsFullscreen(true);
		glfwSetWindowMonitor(m_window, glfwGetPrimaryMonitor(), 0, 0, resX, resY, GLFW_DONT_CARE);
	}
	
	void Window::MakeWindowed()
	{
		if (!m_isFullscreen)
			return;
		m_isFullscreen = false;
		Settings::GetInstance().SetIsFullscreen(false);
		glfwSetWindowMonitor(m_window, nullptr, 0, 0, m_width, m_height, GLFW_DONT_CARE);
	}
}
