#include "entityspawnmenu.h"
#include "../../graphics/ui/uirenderer.h"
#include "../../graphics/ui/font.h"
#include "../../mouse.h"
#include "../../updateinfo.h"
#include "../../utils/utils.h"
#include "../../utils/ioutils.h"
#include "../../world/gameworld.h"
#include "../../world/serialization/parseutils.h"

#include <codecvt>
#include <locale>
#include <algorithm>
#include <imgui.h>

namespace TankGame
{
	EntitySpawnMenu::EntitySpawnMenu()
	{
		m_searchBuffer[0] = '\0';
		
		nlohmann::json presetsDoc = nlohmann::json::parse(ReadFileContents(resDirectoryPath / "entitypresets.json"));
		
		for (nlohmann::json::iterator it = presetsDoc.begin(); it != presetsDoc.end(); ++it)
		{
			try
			{
				m_entries.emplace_back(it.key(), it.value());
			}
			catch (const std::exception& exception)
			{
				GetLogStream() << "[error] Error parsing entity preset '" << it.key() << "': "
				               << exception.what() << "\n";
			}
		}
		
		std::sort(m_entries.begin(), m_entries.end(), [] (const EntityEntry& a, const EntityEntry& b)
		{
			return std::less<std::string>()(a.m_label, b.m_label);
		});
	}
	
	void EntitySpawnMenu::Show(glm::vec2 spawnPosition)
	{
		ImGui::OpenPopup("entitySpawn");
		m_spawnPosition = spawnPosition;
		m_setKeyboardFocus = true;
	}
	
	void EntitySpawnMenu::Render()
	{
		if (ImGui::BeginPopup("entitySpawn"))
		{
			ImGui::Text("Spawn Entity...");
			
			ImGui::InputText("##search", m_searchBuffer.data(), m_searchBuffer.size());
			
			//Sets focus to the search box by default
			if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
			                               !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0)))
			{
				ImGui::SetKeyboardFocusHere(-1);
			}
			
			auto searchStringEnd = std::find(m_searchBuffer.begin(), m_searchBuffer.end(), '\0');
			assert(searchStringEnd != m_searchBuffer.end());
			
			bool anyMatches = false;
			
			for (const EntityEntry& entry : m_entries)
			{
				//Checks the entry against the search query
				if (m_searchBuffer[0] != '\0')
				{
					auto it = std::search(entry.m_label.begin(), entry.m_label.end(), m_searchBuffer.begin(),
					                      searchStringEnd, [] (char a, char b)
					{
						return std::toupper(a) == std::toupper(b);
					});
					
					//If the search query wasn't found in the entry's label, skip this entry
					if (it == entry.m_label.end())
						continue;
				}
				
				anyMatches = true;
				
				if (ImGui::MenuItem(entry.m_label.c_str()))
				{
					const EntityParser::ParseParams params = { &m_gameWorld->GetParticlesManager() };
					
					std::unique_ptr<Entity> entity = entry.m_entityParser->Parse(entry.m_jsonElement, params);
					entity->GetTransform().SetPosition(m_spawnPosition);
					
					entity->EditorSpawned();
					
					m_gameWorld->Spawn(std::move(entity));
				}
			}
			
			if (!anyMatches)
				ImGui::Text("No entities found");
			
			ImGui::EndPopup();
		}
		else
			m_searchBuffer[0] = '\0';
	}
	
	EntitySpawnMenu::EntityEntry::EntityEntry(std::string label, const nlohmann::json& jsonElement)
	    : m_label(label), m_jsonElement(jsonElement)
	{
		std::string entityClass = jsonElement["class"].get<std::string>();
		m_entityParser = GetEntityParser(entityClass);
		
		if (m_entityParser == nullptr)
			throw std::runtime_error("There is no parser for entity class '" + entityClass + "'.");
	}
}
