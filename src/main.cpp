#include "window.h"
#include "settings.h"
#include "audio/almanager.h"
#include "audio/soundsmanager.h"
#include "utils/ioutils.h"
#include "graphics/ui/font.h"

#include "orientedrectangle.h"

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
	argumentData.m_noCursorGrab = false;
	
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-prof") == 0)
			argumentData.m_profiling = true;
		if (strcmp(argv[i], "-nocursorgrab") == 0)
			argumentData.m_noCursorGrab = true;
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
	if (!fs::exists(GetResDirectory()))
	{
		std::cerr << "res directory not found. Needs to be in the same directory as the executable!\n";
		return 1;
	}
	
	if (!glfwInit())
	{
		std::cerr << "Error initializing GLFW.\n";
		return 1;
	}
	
	glfwSetErrorCallback(GLFWErrorCallback);
	
	Settings::DetectVideoModes();
	
	if (FT_Init_FreeType(&TankGame::theFTLibrary) != 0)
	{
		std::cerr << "Error initializing freetype.\n";
		glfwTerminate();
		return 1;
	}
	
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
