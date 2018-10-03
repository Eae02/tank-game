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
		enum class States
		{
			Hidden,
			ShownFirstDraw,
			Shown
		};
		
		States m_state = States::Hidden;
		
		std::function<void(const std::string&)> m_saveCallback;
		
		std::array<char, 256> m_inputBuffer;
	};
}
