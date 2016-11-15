#include "editor.h"
#include "../world/gameworld.h"
#include "../world/lights/ishadowlightsource.h"
#include "../utils/ioutils.h"
#include "../utils/utils.h"
#include "../mouse.h"
#include "../keyboard.h"
#include "../level.h"
#include "../gamemanager.h"
#include "../world/serialization/serializeworld.h"
#include "../world/serialization/deserializeworld.h"

#include <glm/gtc/color_space.hpp>
#include <imgui.h>
#include <fstream>
#include <iomanip>

namespace TankGame
{
	Editor::Editor(GameManager& gameManager)
	    : m_gameManager(gameManager), m_entityTool(*this), m_tilesTool(*this), m_pathEditTool(*this),
	      m_tools{ &m_entityTool, &m_tilesTool, &m_pathEditTool }
	{
		
	}
	
	void Editor::Update(const UpdateInfo& updateInfo)
	{
		m_gameWorld->Update(updateInfo);
		
		bool isCtrlDown = updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_LEFT_CONTROL) ||
		                  updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_RIGHT_CONTROL);
		bool isShiftDown = updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_LEFT_SHIFT) ||
		                   updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_RIGHT_SHIFT);
		
		glm::vec2 worldCursorCoords = m_entityTool.GetNewWorldMouseCoords(updateInfo);
		
		//Moves the camera using the middle mouse button
		if (updateInfo.m_mouse.IsButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE))
		{
			m_focusLocation += m_entityTool.GetOldWorldMouseCoords(updateInfo) - worldCursorCoords;
		}
		
		//Moves the camera using the arrow keys
		const float KEYBOARD_MOVE_SPEED = 7;
		if (updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_UP))
			m_focusLocation.y += updateInfo.m_dt * KEYBOARD_MOVE_SPEED;
		if (updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_DOWN))
			m_focusLocation.y -= updateInfo.m_dt * KEYBOARD_MOVE_SPEED;
		if (updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_LEFT))
			m_focusLocation.x -= updateInfo.m_dt * KEYBOARD_MOVE_SPEED;
		if (updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_RIGHT))
			m_focusLocation.x += updateInfo.m_dt * KEYBOARD_MOVE_SPEED;
		
		if (updateInfo.m_mouse.IsButtonPressed(GLFW_MOUSE_BUTTON_RIGHT) && 
		    !updateInfo.m_mouse.WasButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			if (m_tools[m_currentToolIndex]->OnContextMenuOpen(updateInfo.m_viewInfo, updateInfo.m_mouse.GetPosition(),
			                                                   worldCursorCoords))
			{
				m_contextMenuWorldCoords = worldCursorCoords;
				ImGui::OpenPopup("editorCM");
			}
		}
		
		if (m_currentToolIndex == 2)
		{
			if (updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_ESCAPE) && !updateInfo.m_keyboard.WasKeyDown(GLFW_KEY_ESCAPE))
			{
				m_pathEditTool.StopEditing();
				m_currentToolIndex = 0;
			}
		}
		else
		{
			if (updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_F1) && !updateInfo.m_keyboard.WasKeyDown(GLFW_KEY_F1))
				m_currentToolIndex = 0;
			if (updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_F2) && !updateInfo.m_keyboard.WasKeyDown(GLFW_KEY_F2))
				m_currentToolIndex = 1;
		}
		
		if (updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_F5) && !updateInfo.m_keyboard.WasKeyDown(GLFW_KEY_F5))
			TestLevel();
		
		if (isCtrlDown && updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_S) && !updateInfo.m_keyboard.WasKeyDown(GLFW_KEY_S))
		{
			if (isShiftDown)
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
		if (!updateInfo.m_mouse.IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
			m_sendInputsToPropertyWindow = false;
	}
	
	void Editor::LoadLevel(std::string name)
	{
		fs::path fullPath = Level::GetLevelsPath() / name;
		std::string fullPathString = fullPath.string();
		
		std::ifstream stream(fullPathString, std::ios::binary);
		if (!stream)
			throw std::runtime_error("Error opening file for reading: '" + fullPathString + "'.");
		
		m_gameWorld = DeserializeWorld(stream, GameWorld::Types::Editor);
		
		m_currentLevelName = std::move(name);
		m_isTesting = false;
		m_viewWidth = m_gameWorld->GetViewWidth();
		m_focusLocation = m_gameWorld->GetFocusLocation();
		
		for (EditorTool* tool : m_tools)
			tool->SetGameWorld(*m_gameWorld);
		
		if (!m_editorRenderer.IsNull())
		{
			m_editorRenderer->SetWorld(m_gameWorld.get());
		}
	}
	
	void Editor::Close()
	{
		m_gameWorld = nullptr;
	}
	
	const EditorRenderer& Editor::GetRenderer()
	{
		if (m_editorRenderer.IsNull())
		{
			m_editorRenderer.Construct();
			if (m_gameWorld != nullptr)
				m_editorRenderer->SetWorld(m_gameWorld.get());
		}
		return *m_editorRenderer;
	}
	
	void Editor::OnResize(int newWidth, int newHeight)
	{
		m_uiRenderer.SetWindowDimensions(newWidth, newHeight);
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
				if (ImGui::MenuItem("Entities", "F1", m_currentToolIndex == 0))
					m_currentToolIndex = 0;
				if (ImGui::MenuItem("Tiles", "F2", m_currentToolIndex == 1))
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
				{
					m_gameWorld->UpdateTileShadowCasters();
					
					m_gameWorld->IterateEntities([&] (Entity& entity)
					{
						if (entity.AsLightSource() != nullptr)
							if (IShadowLightSource* shadowLightSource = dynamic_cast<IShadowLightSource*>(&entity))
								shadowLightSource->InvalidateShadowMap();
					});
				}
				
				ImGui::MenuItem("Quad Tree", nullptr, &m_drawQuadTree);
				
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
		
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
		
		if (m_drawQuadTree)
		{
			glm::mat3 W(
					m_halfScreenSize.x, 0.0f, 0.0f,
					0.0f, m_halfScreenSize.y, 0.0f,
					m_halfScreenSize.x, m_halfScreenSize.y, 1.0f
			);
			
			m_quadTreeVisualizer.DrawQuadTree(m_uiRenderer, m_gameWorld->GetQuadTree(), W * viewInfo.GetViewMatrix());
		}
		
		m_tools[m_currentToolIndex]->DrawUI(viewInfo, m_uiRenderer);
		
		glDisable(GL_BLEND);
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
		
		m_gameManager.SetLevel(Level(levelStream), true);
	}
}
