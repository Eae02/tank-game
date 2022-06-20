#include "saveasdialog.h"
#include "../keyboard.h"

#include <imgui.h>

namespace TankGame
{
	SaveAsDialog::SaveAsDialog()
	{
		m_inputBuffer[0] = '\0';
	}
	
	void SaveAsDialog::Render()
	{
		if (!m_visible)
			return;
		
		ImGui::SetNextWindowPos(
			ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y / 2.0f),
			ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		
		bool isOpen = true;
		
		if (ImGui::Begin("Save As...", &isOpen, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize |
		                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			bool done = ImGui::InputText("Filename", m_inputBuffer.data(), m_inputBuffer.size(),
			                             ImGuiInputTextFlags_EnterReturnsTrue);
			
			if (ImGui::Button("OK"))
				done = true;
			
			if (done && m_inputBuffer[0] != '\0')
			{
				m_saveCallback(std::string(m_inputBuffer.data()));
				m_inputBuffer[0] = '\0';
				m_visible = false;
			}
			
			ImGui::SameLine();
			if (ImGui::Button("Cancel") || (ImGui::IsWindowFocused() && ImGui::IsKeyDown((int)Key::Escape)))
				m_visible = false;
		}
		
		ImGui::End();
		
		if (!isOpen)
			m_visible = false;
	}
	
	void SaveAsDialog::Show(std::function<void(const std::string&)> saveCallback)
	{
		m_saveCallback = std::move(saveCallback);
		m_visible = true;
	}
}
