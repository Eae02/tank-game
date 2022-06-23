#include "messagebox.h"

#ifdef __EMSCRIPTEN__

#include <iostream>
#include <emscripten/emscripten.h>

namespace TankGame
{
	void ShowErrorMessage(const std::string& message, const std::string& title)
	{
		EM_ASM({
			displayError(UTF8ToString($0));
		}, message.c_str());
		std::exit(0);
	}
}

#endif
