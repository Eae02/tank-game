#pragma once

#include <string>
#include "filesystem.h"

namespace TankGame
{
	std::string ReadFileContents(const fs::path& path);
	
	const fs::path& GetResDirectory();
	
	const fs::path& GetDataDirectory();
}
