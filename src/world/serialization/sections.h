#pragma once

#include <ostream>
#include <istream>

namespace TankGame
{
	void WriteSection(const char* data, size_t dataSize, std::ostream& stream);
	std::vector<char> ReadSection(std::istream& input);
}
