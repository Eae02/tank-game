#include "imgui.h"

#include <imgui.h>

namespace TankGame
{
	static std::array<char, 1024> scriptBuffer;
	
	bool RenderScriptEditor(const char* label, std::string& code)
	{
		ImGui::PushID(label);
		
		strncpy(scriptBuffer.data(), code.c_str(), scriptBuffer.size());
		scriptBuffer.back() = '\0';
		
		ImGui::TextUnformatted(label);
		
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::PushItemWidth(-1);
		
		bool changed = ImGui::InputTextMultiline("##ScriptEditor", scriptBuffer.data(), scriptBuffer.size(),
		                                         { }, ImGuiInputTextFlags_AllowTabInput);
		
		ImGui::PopItemWidth();
		ImGui::PopFont();
		
		ImGui::PopID();
		
		if (changed)
			code.assign(scriptBuffer.data());
		return changed;
	}
}
