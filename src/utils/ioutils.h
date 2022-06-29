#pragma once

namespace TankGame
{
	std::string ReadFileContents(const fs::path& path);
	
	extern fs::path resDirectoryPath;
	extern fs::path dataDirectoryPath;
}
