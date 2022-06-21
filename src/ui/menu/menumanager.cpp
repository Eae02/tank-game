#include "menumanager.h"
#include "../../world/serialization/deserializeworld.h"
#include "../../utils/random.h"
#include "../../utils/ioutils.h"
#include "../../utils/jsonparseutils.h"
#include "../../level.h"
#include "../../graphics/deferredrenderer.h"
#include "../../updateinfo.h"
#include "../../progress.h"

#include <fstream>
#include <random>
#include <algorithm>
#include <glm/gtc/constants.hpp>

using namespace std::chrono;

namespace TankGame
{
	MenuManager::MenuManager()
	{
		auto backgroundsJson = nlohmann::json::parse(ReadFileContents(resDirectoryPath / "menubackgrounds.json"));
		
		for (const auto& menuBackgroundEl : backgroundsJson)
		{
			try
			{
				m_backgrounds.emplace_back(menuBackgroundEl);
			}
			catch (const std::exception& ex)
			{
				GetLogStream() << LOG_ERROR << "Error parsing menu background: " << ex.what() << "\n";
			}
		}
		
		auto backCallback = [this] { SetCurrentMenu(MenuScreens::MainMenu); };
		m_optionsMenu.m_backCallback = backCallback;
		m_playMenu.SetBackCallback(backCallback);
		
		m_mainMenu.SetPlayCallback([this] { SetCurrentMenu(MenuScreens::Play); });
		m_mainMenu.SetOptionsCallback([this] { SetCurrentMenu(MenuScreens::Options); });
	}
	
	long MenuManager::GetBackgroundIndex(const std::string& levelName) const
	{
		int progress = Progress::GetInstance().GetLevelProgress(levelName);
		
		long bestBackgroundIndex = -1;
		
		for (size_t i = 0; i < m_backgrounds.size(); i++)
		{
			if (m_backgrounds[i].m_levelName == levelName && progress >= m_backgrounds[i].m_requiredProgress)
			{
				if (bestBackgroundIndex == -1 ||
				    m_backgrounds[i].m_requiredProgress > m_backgrounds[bestBackgroundIndex].m_requiredProgress)
				{
					bestBackgroundIndex = static_cast<long>(i);
				}
			}
		}
		
		return bestBackgroundIndex;
	}
	
	void MenuManager::OnResize(int newWidth, int newHeight)
	{
		m_screenWidth = newWidth;
		m_screenHeight = newHeight;
		
		UpdateViewInfo();
		
		m_mainMenu.OnResize(newWidth, newHeight);
		m_optionsMenu.OnResize(newWidth, newHeight);
		m_playMenu.OnResize(newWidth, newHeight);
	}
	
	MenuManager::Background::Background(const nlohmann::json& element)
	    : m_levelName(element["level"].get<std::string>()),
	      m_focusPosition(ParseVec2(element["position"])),
	      m_requiredProgress(element["requiredProgress"]) { }
	
	static std::uniform_real_distribution<float> rotationDist(-glm::pi<float>(), glm::pi<float>());
	
	void MenuManager::ShowMainMenu()
	{
		if (!m_visible)
			m_rotation = rotationDist(globalRNG);
		
		SetCurrentMenu(MenuScreens::MainMenu);
		m_visible = true;
		
		m_currentBackgroundIndex = GetBackgroundIndex(Settings::instance.GetLastPlayedLevelName());
		if (m_currentBackgroundIndex == -1)
			m_currentBackgroundIndex = GetBackgroundIndex("level_1");
		
		if (m_currentBackgroundIndex == -1)
		{
			SetBackgroundWorld(nullptr);
		}
		else
		{
			std::ifstream stream(Level::GetLevelsPath() / m_backgrounds[m_currentBackgroundIndex].m_levelName,
			                     std::ios::binary);
			
			SetBackgroundWorld(DeserializeWorld(stream, GameWorld::Types::MenuBackground));
		}
		
		UpdateViewInfo();
	}
	
	void MenuManager::Hide()
	{
		SetBackgroundWorld(nullptr);
		m_visible = false;
	}
	
	void MenuManager::Update(const UpdateInfo& updateInfo)
	{
		if (m_backgroundWorld != nullptr)
			m_backgroundWorld->Update(updateInfo);
		
		if (m_currentMenu == MenuScreens::MainMenu)
			m_mainMenu.Update(updateInfo);
		else if (m_currentMenu == MenuScreens::Options)
			m_optionsMenu.Update(updateInfo);
		else if (m_currentMenu == MenuScreens::Play)
			m_playMenu.Update(updateInfo);
	}
	
	void MenuManager::Draw(DeferredRenderer& deferredRenderer, ShadowRenderer& shadowRenderer, float gameTime) const
	{
		if (m_backgroundWorld == nullptr)
		{
			glm::vec4 clearColor(ParseColorHexCodeSRGB(0x80ADBF), 1.0f);
			glClearBufferfv(GL_COLOR, 0, reinterpret_cast<const float*>(&clearColor));
		}
		else
		{
			deferredRenderer.SetBlurAmount(1);
			
			m_worldRenderer.DrawShadowMaps(shadowRenderer, m_viewInfo);
			
			m_worldRenderer.Prepare(m_viewInfo, gameTime);
			deferredRenderer.Draw(m_worldRenderer, m_viewInfo);
		}
		
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		
		if (m_currentMenu == MenuScreens::MainMenu)
			m_mainMenu.Draw(UIRenderer::GetInstance());
		else if (m_currentMenu == MenuScreens::Options)
			m_optionsMenu.Draw(UIRenderer::GetInstance());
		else if (m_currentMenu == MenuScreens::Play)
			m_playMenu.Draw(UIRenderer::GetInstance());
		
		glDisable(GL_BLEND);
	}
	
	void MenuManager::SetBackgroundWorld(std::unique_ptr<GameWorld>&& backgroundWorld)
	{
		m_backgroundWorld = std::move(backgroundWorld);
		m_worldRenderer.SetWorld(m_backgroundWorld.get());
	}
	
	void MenuManager::SetCurrentMenu(MenuScreens menu)
	{
		switch (menu)
		{
		case MenuScreens::MainMenu:
			m_mainMenu.OnOpen();
			break;
		case MenuScreens::Options:
			m_optionsMenu.OnOpen();
			break;
		case MenuScreens::Play:
			m_playMenu.OnOpen();
			break;
		}
		
		m_currentMenu = menu;
	}
	
	void MenuManager::UpdateViewInfo()
	{
		if (m_currentBackgroundIndex == -1)
		{
			m_viewInfo = { };
		}
		else
		{
			float ar = static_cast<float>(m_screenWidth) / m_screenHeight;
			m_viewInfo = { m_backgrounds[m_currentBackgroundIndex].m_focusPosition, m_rotation, 20, ar };
		}
	}
}
