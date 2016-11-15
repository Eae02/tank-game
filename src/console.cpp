#include "console.h"
#include "utils/utils.h"

#include <imgui.h>
#include <sstream>
#include <GLFW/glfw3.h>
#include <iostream>

namespace TankGame
{
	extern std::stringstream logStream;
	
	Console::Console()
	{
		m_inputBuffer[0] = '\0';
	}
	
	void Console::Render()
	{
		if (!m_isOpen)
			return;
		
		if (ImGui::Begin("Console", &m_isOpen, ImVec2(600, 400)))
		{
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			
			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), false,
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
				
				ImGui::PushStyleColor(ImGuiCol_Text, color);
				ImGui::TextUnformatted(line.c_str());
				ImGui::PopStyleColor();
			}
			
			logStream.clear();
			
			ImGui::PopStyleVar();
			ImGui::EndChild();
			ImGui::Separator();
			
			bool invoke = ImGui::InputText("##ConsoleInput", m_inputBuffer.data(), m_inputBuffer.size(),
			                               ImGuiInputTextFlags_EnterReturnsTrue);
			
			if (ImGui::IsItemHovered() || (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
				ImGui::SetKeyboardFocusHere(-1);
			
			ImGui::SameLine();
			
			if (ImGui::Button("Invoke"))
				invoke = true;
			
			if (invoke)
			{
				std::vector<std::string> tokens = Split(m_inputBuffer.data(), " ", true);
				if (!tokens.empty())
				{
					logStream << "> " << m_inputBuffer.data() << "\n";
					
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
						try
						{
							commandPos->second.m_callback(tokens.data() + 1, tokens.size() - 1);
						}
						catch (const std::exception& exception)
						{
							logStream << LOG_ERROR << exception.what() << "\n";
						}
					}
				}
				
				m_inputBuffer[0] = '\0';
			}
			
			ImGui::PopFont();
		}
		
		ImGui::End();
	}
}
