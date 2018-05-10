#include "client.h"
#include "window.h"
#include "settings.h"
#include "progress.h"
#include "audio/almanager.h"
#include "audio/soundsmanager.h"
#include "utils/ioutils.h"
#include "exceptions/fatalexception.h"
#include "graphics/ui/font.h"
#include "messagebox.h"
#include "orientedrectangle.h"
#include "lua/luavm.h"

#include <cstring>
#include <iostream>

namespace TankGame
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		throw FatalException(std::to_string(error) + " " + description);
	}
	
	void StartClient(const ClientArgs& args)
	{
		if (!fs::exists(GetResDirectory()))
			throw FatalException("res directory not found. Needs to be in the same directory as the executable!");
		
		if (!glfwInit())
			throw FatalException("Error initializing GLFW.");
		
		glfwSetErrorCallback(GLFWErrorCallback);
		
		Settings::DetectVideoModes();
		
		if (FT_Init_FreeType(&TankGame::theFTLibrary) != 0)
			throw FatalException("Error initializing freetype.");
		
		InitOpenAL();
		
		fs::path progressPath(GetDataDirectory() / "progress.json");
		if (fs::exists(progressPath))
			Progress::SetInstance({ progressPath });
		
		fs::path settingsPath(GetDataDirectory() / "settings.json");
		if (fs::exists(settingsPath))
			Settings::SetInstance(Settings(settingsPath));
		else
			Settings::SetInstance(Settings());
		
		Lua::Init();
		
		{
			Window window(args);
			window.RunGame();
		}
		
		Lua::Destroy();
		
		Settings::GetInstance().Save(settingsPath);
		Progress::GetInstance().Save(progressPath);
		
		SoundsManager::SetInstance(nullptr);
		CloseOpenAL();
		
		FT_Done_FreeType(TankGame::theFTLibrary);
		
		glfwTerminate();
	}
}