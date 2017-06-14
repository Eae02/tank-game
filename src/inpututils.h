#pragma once

#include "updateinfo.h"

namespace TankGame
{
	bool IsButtonPressed(const class Keyboard& keyboard, const class Mouse& mouse, int button);
	
	inline bool IsButtonPressed(const UpdateInfo& updateInfo, int button)
	{
		return IsButtonPressed(updateInfo.m_keyboard, updateInfo.m_mouse, button);
	}
	
	bool WasButtonPressed(const class Keyboard& keyboard, const class Mouse& mouse, int button);
	
	inline bool WasButtonPressed(const UpdateInfo& updateInfo, int button)
	{
		return WasButtonPressed(updateInfo.m_keyboard, updateInfo.m_mouse, button);
	}
	
	inline bool IsButtonPressedNow(const UpdateInfo& updateInfo, int button)
	{
		return IsButtonPressed(updateInfo, button) && !WasButtonPressed(updateInfo, button);
	}
}
