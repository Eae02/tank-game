#ifdef _WIN32
#include "paths.h"

#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <shlobj.h>

namespace TankGame
{
	fs::path GetResPath()
	{
		TCHAR exePathOut[MAX_PATH];
		GetModuleFileName(nullptr, exePathOut, MAX_PATH);
		return fs::path(exePathOut).parent_path() / "res";
	}
	
	fs::path GetDataPath()
	{
		LPWSTR wszPath = NULL;
		SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &wszPath);
		return fs::path(wszPath) / "EaeTankGame";
	}
}

#endif
