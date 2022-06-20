#pragma once

#include "graphics/gl/functions.h"
#include <imgui.h>

namespace TankGame
{
	struct UpdateInfo;
	
	namespace ImGuiInterface
	{
		void Init();
		void NewFrame(const UpdateInfo& updateInfo, float dt);
		void EndFrame();
		void Close();
		
		bool IsMouseCaptured();
		bool IsKeyboardCaptured();
	}
}
