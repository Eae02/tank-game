#pragma once

#include <string>
#include "filesystem.h"

namespace TankGame
{
	std::string ReadFileContents(const fs::path& path);
	
	extern fs::path resDirectoryPath;
	extern fs::path dataDirectoryPath;
}
