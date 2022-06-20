#include "ioutils.h"

#include <cstring>
#include <fstream>
#include <algorithm>

#if defined(__linux__)
#include <libgen.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <shlobj.h>
#endif

namespace TankGame
{
	fs::path theResDirectoryPath;
	fs::path theDataDirectoryPath;
	
	std::string ReadFileContents(const fs::path& path)
	{
		std::string pathString = path.string();
		std::ifstream stream(pathString);
		if (!stream)
			throw std::runtime_error("Error opening file for reading: '" + pathString + "'.");
		
		std::string result;
		
		char buffer[4096];
		while (stream.read(buffer, sizeof(buffer)))
			result.append(buffer, sizeof(buffer));
		
		result.append(buffer, static_cast<size_t>(stream.gcount()));
		
		return result;
	}
	
	const fs::path& GetResDirectory()
	{
		if (theResDirectoryPath.empty())
		{
#if defined(__EMSCRIPTEN__)
			theResDirectoryPath = "/res/";
#elif defined(__linux__)
			char exePathOut[PATH_MAX];
			ssize_t numBytesWritten = readlink("/proc/self/exe", exePathOut, PATH_MAX);
			
			if (numBytesWritten == -1)
				throw std::runtime_error("Error getting path to executable.");
			exePathOut[numBytesWritten] = '\0';
			
			dirname(exePathOut);
			theResDirectoryPath = fs::path(exePathOut) / "res";
#elif defined(_WIN32)
			TCHAR exePathOut[MAX_PATH];
			GetModuleFileName(nullptr, exePathOut, MAX_PATH);
			
			theResDirectoryPath = fs::path(exePathOut).parent_path() / "res";
#endif
		}
		
		return theResDirectoryPath;
	}
	
	const fs::path& GetDataDirectory()
	{
		if (theDataDirectoryPath.empty())
		{
#if defined(__EMSCRIPTEN__)
			theDataDirectoryPath = "/data/";
#elif defined(__linux__)
			const char* LINUX_PATH = ".local/share/TankGame";
			
			if (struct passwd* pwd = getpwuid(getuid()))
				theDataDirectoryPath = fs::path(pwd->pw_dir) / LINUX_PATH;
			else
				theDataDirectoryPath = fs::path(getenv("HOME")) / LINUX_PATH;
#elif defined(_WIN32)
			LPWSTR wszPath = NULL;
			SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &wszPath);
			theDataDirectoryPath = fs::path(wszPath) / "TankGame";
#endif
			
			if (!fs::exists(theDataDirectoryPath))
				fs::create_directories(theDataDirectoryPath);
		}
		
		return theDataDirectoryPath;
	}
}
