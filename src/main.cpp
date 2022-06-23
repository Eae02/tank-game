#include "settings.h"
#include "progress.h"
#include "audio/almanager.h"
#include "audio/soundsmanager.h"
#include "utils/ioutils.h"
#include "graphics/ui/font.h"
#include "platform/messagebox.h"
#include "platform/window.h"
#include "platform/common.h"
#include "platform/paths.h"
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

static VideoModes videoModes;

static bool Initialize()
{
	resDirectoryPath = GetResPath();
	dataDirectoryPath = GetDataPath();
	
	if (!fs::exists(resDirectoryPath))
	{
		resDirectoryPath = fs::current_path() / "res";
		if (!fs::exists(resDirectoryPath))
		{
			ShowErrorMessage(
				"res directory not found.\nThe directory needs to be in the same directory as the executable.",
				"Resource directory not found");
			return false;
		}
	}
	
	if (!fs::exists(dataDirectoryPath))
		fs::create_directories(dataDirectoryPath);
	
	PlatformInitialize();
	
	videoModes = DetectVideoModes();
	
	if (FT_Init_FreeType(&TankGame::theFTLibrary) != 0)
	{
		ShowErrorMessage("Error initializing freetype", "Error");
		return false;
	}
	
	InitOpenAL();
	
	fs::path progressPath(dataDirectoryPath / "progress.json");
	if (fs::exists(progressPath))
		Progress::SetInstance({ progressPath });
	
	fs::path settingsPath(dataDirectoryPath / "settings.json");
	if (fs::exists(settingsPath))
		Settings::instance.Load(settingsPath, videoModes);
	else
		Settings::instance.SetToDefaultVideoMode(videoModes);
	
	Lua::Init();
	
	return true;
}

#ifdef __EMSCRIPTEN__
extern "C" void WebMain()
{
	if (Initialize())
	{
		RunGame(ArgumentData(), videoModes);
	}
}
#else
int main(int argc, const char** argv)
{
	if (!Initialize()) return 1;
	
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
	
	RunGame(argumentData, videoModes);
	
	Lua::Destroy();
	
	Settings::instance.Save(dataDirectoryPath / "settings.json");
	Progress::GetInstance().Save(dataDirectoryPath / "progress.json");
	
	SoundsManager::SetInstance(nullptr);
	CloseOpenAL();
	
	FT_Done_FreeType(TankGame::theFTLibrary);
	
	PlatformShutdown();
}
#endif
