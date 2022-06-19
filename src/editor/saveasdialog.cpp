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
		if (m_state == States::Hidden)
			return;
		
		if (m_state == States::ShownFirstDraw)
		{
			ImGui::SetNextWindowPosCenter();
			m_state = States::Shown;
		}
		
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
				m_state = States::Hidden;
			}
			
			ImGui::SameLine();
			if (ImGui::Button("Cancel") || (ImGui::IsWindowFocused() && ImGui::IsKeyDown((int)Key::Escape)))
				m_state = States::Hidden;
		}
		
		ImGui::End();
		
		if (!isOpen)
			m_state = States::Hidden;
	}
	
	void SaveAsDialog::Show(std::function<void(const std::string&)> saveCallback)
	{
		m_saveCallback = std::move(saveCallback);
		m_state = States::ShownFirstDraw;
	}
}
