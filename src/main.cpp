#include "settings.h"
#include "progress.h"
#include "audio/almanager.h"
#include "audio/soundsmanager.h"
#include "utils/ioutils.h"
#include "exceptions/fatalexception.h"
#include "graphics/ui/font.h"
#include "platform/messagebox.h"
#include "platform/window.h"
#include "platform/common.h"
#include "game.h"
#include "orientedrectangle.h"
#include "lua/luavm.h"

#include <cstring>
#include <iostream>

#ifdef _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
extern "C"
{
	__declspec(dllexport) uint32_t NvOptimusEnablement = 1;
	__declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}
#endif

using namespace TankGame;

static ArgumentData ParseArguments(int argc, const char** argv)
{
	ArgumentData argumentData;
	
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

int Run(int argc, const char** argv)
{
	try
	{
		PlatformInitialize();
		
		VideoModes videoModes = DetectVideoModes();
		
		if (FT_Init_FreeType(&TankGame::theFTLibrary) != 0)
			throw FatalException("Error initializing freetype.");
		
		InitOpenAL();
		
		fs::path progressPath(GetDataDirectory() / "progress.json");
		if (fs::exists(progressPath))
			Progress::SetInstance({ progressPath });
		
		fs::path settingsPath(GetDataDirectory() / "settings.json");
		
		if (fs::exists(settingsPath))
			Settings::instance.Load(settingsPath, videoModes);
		
		Lua::Init();
		
		RunGame(ParseArguments(argc, argv), videoModes);
		
		Lua::Destroy();
		
		Settings::instance.Save(settingsPath);
		Progress::GetInstance().Save(progressPath);
		
		SoundsManager::SetInstance(nullptr);
		CloseOpenAL();
		
		FT_Done_FreeType(TankGame::theFTLibrary);
		
		PlatformShutdown();
	}
	catch (const FatalException& exception)
	{
		ShowErrorMessage(exception.what(), "Error");
		return 1;
	}
#ifdef NDEBUG
	catch (const std::exception& exception)
	{
		ShowErrorMessage(exception.what(), "Unexpected Error");
		return 1;
	}
#endif
	
	return 0;
}
 
#ifdef __EMSCRIPTEN__
extern "C" void WebMain()
{
	const char* args[] = { "tank-game", "-dsawrapper", nullptr };
	Run(2, args);
}
#else
int main(int argc, const char** argv)
{
	if (!fs::exists(GetResDirectory()))
	{
		ShowErrorMessage(
			"res directory not found.\nThe directory needs to be in the same directory as the executable.",
			"Resource directory not found");
		return 1;
	}
	return Run(argc, argv);
}
#endif
