#include "editor.h"
#include "../world/gameworld.h"
#include "../utils/ioutils.h"
#include "../utils/utils.h"
#include "../mouse.h"
#include "../keyboard.h"
#include "../level.h"
#include "../gamemanager.h"
#include "../world/serialization/serializeworld.h"
#include "../world/serialization/deserializeworld.h"

#include <imgui.h>
#include <fstream>
#include <iomanip>

namespace TankGame
{
	Editor::Editor(GameManager& gameManager)
	    : m_gameManager(gameManager), m_entityTool(*this), m_tilesTool(*this), m_pathEditTool(*this),
	      m_tools{ &m_entityTool, &m_tilesTool, &m_pathEditTool } { }
	
	void Editor::Update(const UpdateInfo& updateInfo)
	{
		m_gameWorld->Update(updateInfo);
		
		glm::vec2 worldCursorCoords = m_entityTool.GetNewWorldMouseCoords(updateInfo);
		
		//Moves the camera using the middle mouse button
		if (updateInfo.m_mouse.IsDown(MouseButton::Middle))
		{
			m_focusLocation += m_entityTool.GetOldWorldMouseCoords(updateInfo) - worldCursorCoords;
		}
		
		//Moves the camera using the arrow keys
		const float KEYBOARD_MOVE_SPEED = 7;
		if (updateInfo.m_keyboard.IsDown(Key::ArrowUp))
			m_focusLocation.y += updateInfo.m_dt * KEYBOARD_MOVE_SPEED;
		if (updateInfo.m_keyboard.IsDown(Key::ArrowDown))
			m_focusLocation.y -= updateInfo.m_dt * KEYBOARD_MOVE_SPEED;
		if (updateInfo.m_keyboard.IsDown(Key::ArrowLeft))
			m_focusLocation.x -= updateInfo.m_dt * KEYBOARD_MOVE_SPEED;
		if (updateInfo.m_keyboard.IsDown(Key::ArrowRight))
			m_focusLocation.x += updateInfo.m_dt * KEYBOARD_MOVE_SPEED;
		
		if (updateInfo.m_mouse.IsDown(MouseButton::Right) && !updateInfo.m_mouse.WasDown(MouseButton::Right))
		{
			if (m_tools[m_currentToolIndex]->OnContextMenuOpen(updateInfo.m_viewInfo, updateInfo.m_mouse.pos,
			                                                   worldCursorCoords))
			{
				m_contextMenuWorldCoords = worldCursorCoords;
				ImGui::OpenPopup("editorCM");
			}
		}
		
		if (m_currentToolIndex == 2)
		{
			if (updateInfo.m_keyboard.IsDown(Key::Escape) && !updateInfo.m_keyboard.WasDown(Key::Escape))
			{
				m_pathEditTool.StopEditing();
				m_currentToolIndex = 0;
			}
		}
		else
		{
			if (updateInfo.m_keyboard.IsDown(Key::F1) && !updateInfo.m_keyboard.WasDown(Key::F1))
				m_currentToolIndex = 0;
			if (updateInfo.m_keyboard.IsDown(Key::F2) && !updateInfo.m_keyboard.WasDown(Key::F2))
				m_currentToolIndex = 1;
		}
		
		if (updateInfo.m_keyboard.IsDown(Key::F5) && !updateInfo.m_keyboard.WasDown(Key::F5))
			TestLevel();
		
		if (updateInfo.m_keyboard.IsAnyDown(KEY_MASK_CONTROL) && updateInfo.m_keyboard.IsDown(Key::S) && !updateInfo.m_keyboard.WasDown(Key::S))
		{
			if (updateInfo.m_keyboard.IsAnyDown(KEY_MASK_SHIFT))
				SaveAs();
			else
				Save();
		}
		
		for (long i = m_propertyWindows.size() - 1; i >= 0; i--)
		{
			if (m_propertyWindows[i].ShouldClose())
			{
				m_propertyWindows.erase(m_propertyWindows.begin() + i);
			}
		}
		
		if (!m_sendInputsToPropertyWindow)
			m_tools[m_currentToolIndex]->Update(updateInfo);
		if (!updateInfo.m_mouse.IsDown(MouseButton::Left))
			m_sendInputsToPropertyWindow = false;
	}
	
	bool Editor::LoadLevel(std::string name)
	{
		fs::path fullPath = Level::GetLevelsPath() / name;
		std::string fullPathString = fullPath.string();
		
		std::ifstream stream(fullPathString, std::ios::binary);
		if (!stream)
			return false;
		
		m_gameWorld = DeserializeWorld(stream, GameWorld::Types::Editor);
		
		m_currentLevelName = std::move(name);
		m_isTesting = false;
		m_viewWidth = m_gameWorld->GetViewWidth();
		m_focusLocation = m_gameWorld->GetFocusLocation();
		
		for (EditorTool* tool : m_tools)
			tool->SetGameWorld(*m_gameWorld);
		
		if (m_editorRenderer != nullptr)
			m_editorRenderer->SetWorld(m_gameWorld.get());
		
		return true;
	}
	
	void Editor::Close()
	{
		m_gameWorld = nullptr;
	}
	
	const EditorRenderer& Editor::GetRenderer()
	{
		if (m_editorRenderer == nullptr)
		{
			m_editorRenderer = std::make_unique<EditorRenderer>();
			if (m_gameWorld != nullptr)
				m_editorRenderer->SetWorld(m_gameWorld.get());
		}
		return *m_editorRenderer;
	}
	
	void Editor::OnResize(int newWidth, int newHeight)
	{
		m_halfScreenSize = { newWidth / 2.0f, newHeight / 2.0f };
		
		for (EditorTool* tool : m_tools)
			tool->OnResize(newWidth, newHeight);
	}
	
	void Editor::DrawUI(const ViewInfo& viewInfo)
	{
		m_focusedPropertiesObject = nullptr;
		for (PropertiesWindow& propertiesWindow : m_propertyWindows)
		{
			propertiesWindow.Render();
			if (propertiesWindow.HasFocus())
				m_focusedPropertiesObject = propertiesWindow.GetObject();
		}
		
		m_saveAsDialog.Render();
		
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save", "Ctrl+S"))
					Save();
				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
					SaveAs();
				
				ImGui::EndMenu();
			}
			
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Despawn Entities", "X", false, m_currentToolIndex == 0))
					m_entityTool.DespawnSelected();
				
				ImGui::EndMenu();
			}
			
			if (ImGui::BeginMenu("Tools"))
			{
				bool sel;
				
				sel = m_currentToolIndex == 0;
				if (ImGui::MenuItem("Entities", "F1", &sel))
					m_currentToolIndex = 0;
				
				sel = m_currentToolIndex == 1;
				if (ImGui::MenuItem("Tiles", "F2", sel))
					m_currentToolIndex = 1;
				
				ImGui::Separator();
				
				if (ImGui::MenuItem("Test Level", "F5"))
				{
					TestLevel();
				}
				
				ImGui::EndMenu();
			}
			
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::SliderFloat("Field of View", &m_viewWidth, 1.0f, 100.0f))
					m_viewWidth = glm::clamp(m_viewWidth, 1.0f, 100.0f);
				
				if (ImGui::MenuItem("Update Shadows"))
					UpdateShadows();
				
				if (ImGui::BeginMenu("Lighting"))
				{
					bool gameLighting = m_editorRenderer->UseGameLighting();
					if (ImGui::MenuItem("Editor", nullptr, !gameLighting))
						gameLighting = false;
					if (ImGui::MenuItem("Game", nullptr, gameLighting))
						gameLighting = true;
					if (gameLighting != m_editorRenderer->UseGameLighting())
						m_editorRenderer->SetUseGameLighting(gameLighting);
					
					ImGui::EndMenu();
				}
				
				m_tools[m_currentToolIndex]->RenderViewMenu();
				
				ImGui::EndMenu();
			}
			
			ImGui::EndMainMenuBar();
		}
		
		if (ImGui::BeginPopup("editorCM"))
		{
			if (ImGui::MenuItem("Test Level From Here"))
			{
				Entity* playerEntity = m_gameWorld->GetEntityByName("player");
				glm::vec2 oldPlayerPos = playerEntity->GetTransform().GetPosition();
				playerEntity->GetTransform().SetPosition(m_contextMenuWorldCoords);
				
				TestLevel();
				
				playerEntity->GetTransform().SetPosition(oldPlayerPos);
			}
			
			m_tools[m_currentToolIndex]->RenderContextMenu();
			
			ImGui::EndPopup();
		}
		
		//Renders the editor overlay in the top left corner
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8f);
		ImGui::SetNextWindowPos(ImVec2(10, ImGui::GetTextLineHeightWithSpacing() + 10));
		if (ImGui::Begin("###Overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings |
		                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
		{
			ImGui::Text("Camera Focus: %.1f, %.1f", m_focusLocation.x, m_focusLocation.y);
		}
		ImGui::End();
		ImGui::PopStyleVar(1);
		
		m_tools[m_currentToolIndex]->DrawUI(viewInfo, UIRenderer::GetInstance());
	}
	
	void Editor::ShowPropertiesWindow(IPropertiesObject& object)
	{
		bool exists = std::any_of(m_propertyWindows.begin(), m_propertyWindows.end(), [&] (const PropertiesWindow& w)
		{
			return w.GetObject() == &object;
		});
		
		if (!exists)
		{
			m_propertyWindows.emplace_back(&object);
		}
	}
	
	void Editor::EditPath(class IEditablePathProvider& provider)
	{
		m_pathEditTool.SetPathProvider(&provider);
		m_currentToolIndex = 2;
	}
	
	void Editor::UpdateShadows()
	{
		m_gameWorld->UpdateTileShadowCasters();
		
		m_gameWorld->IterateEntities([&] (Entity& entity)
		{
			if (ILightSource* lightSource = entity.AsLightSource())
				lightSource->InvalidateShadowMap();
		});
	}
	
	void Editor::Save()
	{
		std::ofstream stream((Level::GetLevelsPath() / m_currentLevelName).string(), std::ios::binary);
		SerializeWorld(*m_gameWorld, stream);
	}
	
	void Editor::SaveAs()
	{
		m_saveAsDialog.Show([this] (const std::string& fileName)
		{
			m_currentLevelName = fileName;
			Save();
		});
	}
	
	void Editor::TestLevel()
	{
		m_isTesting = true;
		
		std::stringstream levelStream;
		SerializeWorld(*m_gameWorld, levelStream);
		
		levelStream.seekg(0);
		
		Level level(levelStream);
		
		if (!m_currentLevelName.empty())
		{
			fs::path scriptPath = Level::GetLevelsPath() / (m_currentLevelName + ".lua");
			if (fs::exists(scriptPath))
				level.RunScript(scriptPath);
		}
		
		m_gameManager.SetLevel(std::move(level), true);
	}
}
