#include "messagebox.h"

#if !defined(__linux__) && !defined(_WIN32)

#include <iostream>

namespace TankGame
{
	void ShowErrorMessage(const std::string& message, const std::string& title)
	{
		std::cerr << message << std::endl;
		std::abort();
	}
}

#endif
