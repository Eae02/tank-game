#include "messagebox.h"

#ifdef _WIN32

#include <windows.h>

namespace TankGame
{
	void ShowErrorMessage(const std::string& message, const std::string& title)
	{
		MessageBox(nullptr, message.c_str(), title.c_str(), MB_TASKMODAL | MB_ICONERROR | MB_OK);
	}
}

#endif
