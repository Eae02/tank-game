#include "ioutils.h"
#include "utils.h"

#include <cstring>
#include <fstream>
#include <algorithm>

namespace TankGame
{
	fs::path resDirectoryPath;
	fs::path dataDirectoryPath;
	
	std::string ReadFileContents(const fs::path& path)
	{
		std::string pathString = path.string();
		std::ifstream stream(pathString, std::ios::binary);
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
