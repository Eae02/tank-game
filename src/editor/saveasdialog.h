#pragma once

#include <functional>
#include <string>
#include <array>

namespace TankGame
{
	class SaveAsDialog
	{
	public:
		SaveAsDialog();
		
		void Render();
		
		void Show(std::function<void(const std::string&)> saveCallback);
		
	private:
		bool m_visible = false;
		
		std::function<void(const std::string&)> m_saveCallback;
		
		std::array<char, 256> m_inputBuffer;
	};
}
