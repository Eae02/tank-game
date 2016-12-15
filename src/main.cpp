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
		if (strcmp(argv[i], "-dsawrapper") == 0)
			argumentData.m_useDSAWrapper = true;
	}
	
	return argumentData;
}

static void GLFWErrorCallback(int error, const char* description)
{
	throw FatalException(std::to_string(error) + " " + description);
}

int main(int argc, const char** argv)
{
	try
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
		
		{
			Window window(ParseArguments(argc, argv));
			window.RunGame();
		}
		
		Settings::GetInstance().Save(settingsPath);
		Progress::GetInstance().Save(progressPath);
		
		SoundsManager::SetInstance(nullptr);
		CloseOpenAL();
		
		FT_Done_FreeType(TankGame::theFTLibrary);
		
		glfwTerminate();
	}
	catch (const FatalException& exception)
	{
		ShowErrorMessage(exception.what(), "Error");
		return 1;
	}
}
