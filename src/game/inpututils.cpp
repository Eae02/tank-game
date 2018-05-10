#include "inpututils.h"
#include "keyboard.h"
#include "mouse.h"

namespace TankGame
{
	bool IsButtonPressed(const Keyboard& keyboard, const Mouse& mouse, int button)
	{
		if (button < 7)
			return mouse.IsButtonPressed(button);
		return keyboard.IsKeyDown(button);
	}
	
	bool WasButtonPressed(const Keyboard& keyboard, const Mouse& mouse, int button)
	{
		if (button < 7)
			return mouse.WasButtonPressed(button);
		return keyboard.WasKeyDown(button);
	}
}
