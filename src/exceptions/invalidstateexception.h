#pragma once

#include <stdexcept>

namespace TankGame
{
	class InvalidStateException : public std::runtime_error
	{
	public:
		explicit inline InvalidStateException(const std::string& message)
		    : std::runtime_error(message) { }
		
		explicit inline InvalidStateException(const char* message)
		    : std::runtime_error(message) { }
	};
}
