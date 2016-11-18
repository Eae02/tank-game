#include "window.h"
#include "settings.h"
#include "audio/almanager.h"
#include "audio/soundsmanager.h"
#include "utils/ioutils.h"
#include "graphics/ui/font.h"

#include <cstring>
#include <iostream>

#ifdef _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

using namespace TankGame;

static ArgumentData ParseArguments(int argc, const char** argv)
{
	ArgumentData argumentData;
	argumentData.m_profiling = false;
	
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-prof") == 0)
			argumentData.m_profiling = true;
	}
	
	return argumentData;
}

static void GLFWErrorCallback(int error, const char* description)
{
	throw std::runtime_error(std::to_string(error) + " " + description);
}

Settings GetDefaultSettings()
{
	Settings settings;
	
	const GLFWvidmode* videoMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	settings.SetResolution({ static_cast<int>(videoMode->width * 0.8), static_cast<int>(videoMode->height * 0.8) });
	
	return settings;
}

int main(int argc, const char** argv)
{
	if (!glfwInit())
		throw std::runtime_error("Error initializing GLFW.");
	glfwSetErrorCallback(GLFWErrorCallback);
	
	Settings::DetectVideoModes();
	
	if (FT_Init_FreeType(&TankGame::theFTLibrary) != 0)
		throw std::runtime_error("Error initializing freetype.");
	
	InitOpenAL();
	
	fs::path settingsPath(GetDataDirectory() / "settings.json");
	if (fs::exists(settingsPath))
		Settings::SetInstance(Settings(settingsPath));
	else
		Settings::SetInstance(GetDefaultSettings());
	
	{
		Window window(ParseArguments(argc, argv));
		window.RunGame();
	}
	
	Settings::GetInstance().Save(settingsPath);
	
	SoundsManager::SetInstance(nullptr);
	CloseOpenAL();
	
	FT_Done_FreeType(TankGame::theFTLibrary);
	
	glfwTerminate();
}
