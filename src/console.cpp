#include "console.h"
#include "utils/utils.h"

#include <imgui.h>
#include <sstream>
#include <iostream>

namespace TankGame
{
	extern std::stringstream logStream;
	
	Console::Console()
	{
		m_inputBuffer[0] = '\0';
	}
	
	void Console::TextInputCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory)
		{
			const long oldHistoryPosition = m_historyPosition;
			
			if (data->EventKey == ImGuiKey_UpArrow)
			{
				if (m_historyPosition == -1)
					m_historyPosition = static_cast<long>(m_history.size()) - 1;
				else if (m_historyPosition > 0)
					m_historyPosition--;
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
				if (m_historyPosition != -1)
				{
					if (++m_historyPosition >= static_cast<long>(m_history.size()))
						m_historyPosition = -1;
				}
			}
			
			if (oldHistoryPosition != m_historyPosition)
			{
				const char* text = (m_historyPosition >= 0) ? m_history[m_historyPosition].c_str() : "";
				
				data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen = 
				        static_cast<int>(snprintf(data->Buf, static_cast<size_t>(data->BufSize), "%s", text));
				data->BufDirty = true;
			}
		}
	}
	
	static std::vector<std::string> SplitCommand(const std::string& command)
	{
		std::vector<std::string> result;
		
		size_t partBeginPos = 0;
		for (size_t i = 0; i <= command.size(); i++)
		{
			bool isLastChar = i == command.size();
			
			if (command[partBeginPos] == '"')
			{
				if (i != partBeginPos && (isLastChar || command[i] == '"'))
				{
					result.push_back(command.substr(partBeginPos + 1, i - partBeginPos - 1));
					partBeginPos = i + 1;
				}
			}
			else if (isLastChar || command[i] == ' ')
			{
				if (i != partBeginPos)
					result.push_back(command.substr(partBeginPos, i - partBeginPos));
				partBeginPos = i + 1;
			}
		}
		
		return result;
	}
	
	void Console::RunCommand(const std::string& command)
	{
		m_history.push_back(command);
		m_historyPosition = -1;
		
		std::vector<std::string> tokens = SplitCommand(command);
		
		if (!tokens.empty())
		{
			logStream << "> " << command << "\n";
			
			auto commandPos = m_commands.find(tokens[0]);
			
			if (commandPos == m_commands.end())
			{
				logStream << LOG_ERROR << "Command not found\n";
			}
			else if (static_cast<int>(tokens.size()) - 1 < commandPos->second.m_minArguments)
			{
				logStream << LOG_ERROR << "Not enough arguments passed. The command takes at least " << 
				          commandPos->second.m_minArguments << " arguments.\n";
			}
			else
			{
				commandPos->second.m_callback(tokens.data() + 1, tokens.size() - 1);
			}
		}
	}
	
	void Console::Render()
	{
		if (!m_isOpen)
			return;
		
		ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_Appearing);
		if (ImGui::Begin("Console", &m_isOpen))
		{
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			
			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false,
			                  ImGuiWindowFlags_HorizontalScrollbar);
			
			logStream.seekg(0, std::ios::beg);
			
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
			for (std::string line; std::getline(logStream, line);)
			{
				ImVec4 color(1.0f, 1.0f, 1.0f, 1.0f);
				
				if (!line.compare(0, LOG_ERROR.size(), LOG_ERROR))
					color = { 1.0f, 0.4f, 0.4f, 1.0f };
				if (!line.compare(0, LOG_WARNING.size(), LOG_WARNING))
					color = { 1.0f, 0.65f, 0.2f, 1.0f };
				if (!line.compare(0, LOG_PERFORMANCE.size(), LOG_PERFORMANCE))
					color = { 0.0f, 0.6f, 0.8f, 1.0f };
				
				ImGui::PushStyleColor(ImGuiCol_Text, color);
				ImGui::TextUnformatted(line.c_str());
				ImGui::PopStyleColor();
			}
			
			logStream.clear();
			
			ImGui::PopStyleVar();
			ImGui::EndChild();
			ImGui::Separator();
			
			ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 100);
			
			bool invoke = ImGui::InputText("##ConsoleInput", m_inputBuffer.data(), m_inputBuffer.size(),
			                               ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory,
			                               [] (ImGuiInputTextCallbackData* data)
			{
				reinterpret_cast<Console*>(data->UserData)->TextInputCallback(data);
				return 0;
			}, reinterpret_cast<void*>(this));
			
			if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
				ImGui::SetKeyboardFocusHere(-1);
			
			ImGui::PopItemWidth();
			
			ImGui::SameLine();
			
			ImGui::PushItemWidth(-1);
			
			if (ImGui::Button("Invoke"))
				invoke = true;
			
			if (invoke)
			{
				RunCommand(m_inputBuffer.data());
				m_inputBuffer[0] = '\0';
			}
			
			ImGui::PopItemWidth();
			
			ImGui::PopFont();
		}
		
		ImGui::End();
	}
}
