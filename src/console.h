#pragma once

#include <array>
#include <string>
#include <functional>
#include <vector>
#include <unordered_map>

class ImGuiTextEditCallbackData;

namespace TankGame
{
	class Console
	{
	public:
		using CommandCallback = std::function<void(const std::string* argv, size_t argc)>;
		
		Console();
		
		void Render();
		
		inline void AddCommand(std::string name, CommandCallback callback, int minArguments = -1)
		{ m_commands.emplace(std::move(name), CommandEntry(std::move(callback), minArguments)); }
		
		inline void Show()
		{ m_isOpen = true; }
		
	private:
		void TextInputCallback(ImGuiTextEditCallbackData* data);
		void RunCommand(const std::__cxx11::string& command);
		
		struct CommandEntry
		{
			CommandCallback m_callback;
			int m_minArguments;
			
			inline CommandEntry(CommandCallback callback, int minArguments)
			    : m_callback(callback), m_minArguments(minArguments) { }
		};
		
		std::unordered_map<std::string, CommandEntry> m_commands;
		
		std::array<char, 256> m_inputBuffer;
		
		std::vector<std::string> m_history;
		long m_historyPosition = -1;
		
		bool m_isOpen = false;
	};
}
