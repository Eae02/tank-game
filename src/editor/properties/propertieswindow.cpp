#include "propertieswindow.h"

#include <imgui.h>
#include <sstream>

namespace TankGame
{
	uint64_t PropertiesWindow::s_nextWindowID = 0;
	
	PropertiesWindow::PropertiesWindow(IPropertiesObject* object)
	    : m_windowID(s_nextWindowID)
	{
		SetObject(object);
		s_nextWindowID++;
	}
	
	void PropertiesWindow::SetObject(IPropertiesObject* object)
	{
		std::ostringstream labelStream;
		
		if (object != nullptr)
			labelStream << object->GetObjectName();
		else
			labelStream << "Properties Window";
		
		labelStream << "###" << m_windowID;
		m_label = labelStream.str();
		
		m_object = object;
	}
	
	void PropertiesWindow::Render()
	{
		if (m_setPositionToCursor)
		{
			ImGui::SetNextWindowPos(ImGui::GetIO().MousePos);
			m_setPositionToCursor = false;
		}
		
		if (ImGui::Begin(m_label.c_str(), &m_isOpen, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_HorizontalScrollbar))
		{
			if (m_object == nullptr)
				ImGui::Text("No object assigned.");
			else
				m_object->RenderProperties();
		}
		
		if (m_resetSize)
		{
			ImGui::SetWindowSize(ImVec2(300.0f, 400.0f));
			m_resetSize = false;
		}
		
		m_hasFocus = ImGui::IsWindowFocused();
		
		ImGui::End();
	}
}
