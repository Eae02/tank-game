#include "menumanager.h"
#include "../../world/serialization/deserializeworld.h"
#include "../../utils/ioutils.h"
#include "../../utils/jsonparseutils.h"
#include "../../level.h"
#include "../../graphics/deferredrenderer.h"
#include "../../updateinfo.h"

#include <fstream>
#include <algorithm>

namespace TankGame
{
	MenuManager::MenuManager()
	{
		auto backgroundsJson = nlohmann::json::parse(ReadFileContents(GetResDirectory() / "menubackgrounds.json"));
		
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
		
		m_optionsMenu.SetBackCallback([this] { SetCurrentMenu(MenuScreens::MainMenu); });
		
		m_mainMenu.SetOptionsCallback([this] { SetCurrentMenu(MenuScreens::Options); });
	}
	
	void MenuManager::SetBackground(const std::string& name)
	{
		auto pos = std::find_if(m_backgrounds.begin(), m_backgrounds.end(), [&] (const Background& background)
		{ return background.m_name == name; });
		
		if (pos == m_backgrounds.end())
		{
			GetLogStream() << LOG_ERROR << "There is no menu background called '" << name << "'.\n";
			m_currentBackgroundIndex = -1;
			SetBackgroundWorld(nullptr);
		}
		else
		{
			m_currentBackgroundIndex = pos - m_backgrounds.begin();
			LoadBackgroundWorld();
		}
		
		UpdateViewInfo();
	}
	
	void MenuManager::OnResize(int newWidth, int newHeight)
	{
		m_screenWidth = newWidth;
		m_screenHeight = newHeight;
		
		UpdateViewInfo();
		
		m_mainMenu.OnResize(newWidth, newHeight);
		m_optionsMenu.OnResize(newWidth, newHeight);
	}
	
	MenuManager::Background::Background(const nlohmann::json& element)
	    : m_name(element["name"].get<std::string>()),
	      m_levelPath((Level::GetLevelsPath() / element["level"].get<std::string>()).string()),
	      m_focusPosition(ParseVec2(element["position"])) { }
	
	void MenuManager::ShowMainMenu()
	{
		SetCurrentMenu(MenuScreens::MainMenu);
		m_visible = true;
		
		if (m_backgroundWorld == nullptr && m_currentBackgroundIndex != -1)
			LoadBackgroundWorld();
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
	}
	
	void MenuManager::Draw(DeferredRenderer& deferredRenderer) const
	{
		if (m_backgroundWorld == nullptr)
		{
			glm::vec4 clearColor(ParseColorHexCodeSRGB(0x80ADBF), 1.0f);
			glClearBufferfv(GL_COLOR, 0, reinterpret_cast<const float*>(&clearColor));
		}
		else
		{
			deferredRenderer.SetBlurAmount(1);
			
			m_worldRenderer.Prepare(m_viewInfo);
			deferredRenderer.Draw(m_worldRenderer, m_viewInfo);
		}
		
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		
		if (m_currentMenu == MenuScreens::MainMenu)
			m_mainMenu.Draw(UIRenderer::GetInstance());
		else if (m_currentMenu == MenuScreens::Options)
			m_optionsMenu.Draw(UIRenderer::GetInstance());
		
		glDisable(GL_BLEND);
	}
	
	void MenuManager::LoadBackgroundWorld()
	{
		std::ifstream stream(m_backgrounds[m_currentBackgroundIndex].m_levelPath, std::ios::binary);
		SetBackgroundWorld(DeserializeWorld(stream, GameWorld::Types::MenuBackground));
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
			m_viewInfo = { m_backgrounds[m_currentBackgroundIndex].m_focusPosition, 0, 20, ar };
		}
	}
}
