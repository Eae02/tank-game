#pragma once

#include <stdexcept>

namespace TankGame
{
	class FatalException : public std::runtime_error
	{
	public:
		explicit inline FatalException(const std::string& message)
		    : std::runtime_error(message) { }
		
		explicit inline FatalException(const char* message)
		    : std::runtime_error(message) { }
	};
}
