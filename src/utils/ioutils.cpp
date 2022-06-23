#include "ioutils.h"
#include "utils.h"

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
	fs::path resDirectoryPath;
	fs::path dataDirectoryPath;
	
	std::string ReadFileContents(const fs::path& path)
	{
		std::string pathString = path.string();
		std::ifstream stream(pathString);
		if (!stream)
			Panic("Error opening file for reading: '" + pathString + "'.");
		
		std::string result;
		
		char buffer[4096];
		while (stream.read(buffer, sizeof(buffer)))
			result.append(buffer, sizeof(buffer));
		
		result.append(buffer, static_cast<size_t>(stream.gcount()));
		
		return result;
	}
}
