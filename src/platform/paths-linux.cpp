#ifdef __linux__
#include "paths.h"

#include <libgen.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <string.h>
#include <stdexcept>

namespace TankGame
{
	fs::path GetResPath()
	{
		char pathOut[PATH_MAX];
		ssize_t numBytesWritten = readlink("/proc/self/exe", pathOut, PATH_MAX);
		if (numBytesWritten == -1)
			throw std::runtime_error("Error getting path to executable.");
		pathOut[numBytesWritten] = '\0';
		dirname(pathOut);
		return fs::path(pathOut) / "res";
	}
	
	fs::path GetDataPath()
	{
		char* homeDir = nullptr;
		if (passwd* pwd = getpwuid(getuid()))
			homeDir = pwd->pw_dir;
		else
			homeDir = getenv("HOME");
		if (homeDir)
			return fs::path(homeDir) / ".local/share/EaeTankGame";
		return fs::path(".");
	}
}

#endif
