#include "entitytool.h"
#include "../editor.h"
#include "../ieditoruientity.h"
#include "../ieditablepathprovider.h"
#include "../../world/entities/eventbox.h"
#include "../../world/entities/enemies/rocketturret.h"
#include "../../world/gameworld.h"
#include "../../graphics/ui/font.h"
#include "../../utils/ioutils.h"
#include "../../mouse.h"
#include "../../keyboard.h"
#include "../../platform/common.h"
#include "../../utils/utils.h"
#include "../../utils/mathutils.h"
#include "../../world/lights/pointlightentity.h"
#include "../../world/lights/spotlightentity.h"

#include <glm/gtc/color_space.hpp>
#include <algorithm>
#include <imgui.h>

namespace TankGame
{
	EntityTool::EntityTool(Editor& editor)
	    : EditorTool(editor), m_entityIcon(Texture2D::FromFile(resDirectoryPath / "ui" / "entity.png")),
	      m_moveIcon(Texture2D::FromFile(resDirectoryPath / "ui" / "move.png"))
	{
		
	}
	
	static const float ENTITY_ICON_WIDTH = 64;
	static const float MOVE_ICON_WIDTH = 64;
	
	Rectangle EntityTool::GetIconRectangle(const ViewInfo& viewInfo, Entity& entity)
	{
		glm::vec2 halfScreenSize(GetWindowWidth() / 2.0f, GetWindowHeight() / 2.0f);
		
		glm::vec2 viewSpacePos(viewInfo.GetViewMatrix() * glm::vec3(entity.GetTransform().GetPosition(), 1.0));
		glm::vec2 screenSpacePos = (viewSpacePos + glm::vec2(1.0f)) * halfScreenSize;
		
		return Rectangle::CreateCentered(screenSpacePos, ENTITY_ICON_WIDTH / 2, ENTITY_ICON_WIDTH / 2);
	}
	
	template <typename CallbackTp>
	void IterateEntities(EntityTool& self, const ViewInfo& viewInfo, CallbackTp callback)
	{
		self.GetGameWorld().IterateIntersectingEntities(viewInfo.GetViewRectangle(), [&] (Entity& entity)
		{
			if (!entity.IsEditorVisible())
				return;
			callback(entity, self.GetIconRectangle(viewInfo, entity));
		});
	}
	
	void EntityTool::Update(const UpdateInfo& updateInfo)
	{
		glm::vec2 halfScreenSize(GetWindowWidth() / 2, GetWindowHeight() / 2);
		
		glm::vec2 selectionCenterWS;
		for (const SelectedEntity& entity : m_selectedEntities)
			selectionCenterWS += entity.m_entity->GetTransform().GetPosition();
		selectionCenterWS /= m_selectedEntities.size();
		glm::vec2 selectionCenterVS = updateInfo.m_viewInfo.WorldToScreen(selectionCenterWS) * halfScreenSize * 2.0f;
		m_moveIconRectangle = Rectangle::CreateCentered(selectionCenterVS, MOVE_ICON_WIDTH / 2, MOVE_ICON_WIDTH / 2);
		
		glm::vec2 mousePositionWS = GetNewWorldMouseCoords(updateInfo);
		
		if (updateInfo.m_mouse.IsDown(MouseButton::Left))
		{
			if (m_wasMouseLeftPressed)
			{
				if (LengthSquared(updateInfo.m_mouse.pos - updateInfo.m_mouse.GetOldPosition()) > 1E-6)
				{
					if (m_isMovingSelected)
					{
						glm::vec2 moveVector = mousePositionWS - m_moveBeginMousePosWs;
						
						for (const SelectedEntity& entity : m_selectedEntities)
						{
							glm::vec2 newPosition = entity.m_moveBeginLocation + moveVector;
							
							if (updateInfo.m_keyboard.IsAnyDown(KEY_MASK_SHIFT))
							{
								newPosition.x = std::round(newPosition.x * 2.0f) / 2.0f;
								newPosition.y = std::round(newPosition.y * 2.0f) / 2.0f;
							}
							
							entity.m_entity->GetTransform().SetPosition(newPosition);
							entity.m_entity->EditorMoved();
						}
					}
					else
					{
						m_selectionRect = Rectangle::FromMinMax(m_selectionBegin, updateInfo.m_mouse.pos);
						m_isSelecting = true;
						
						m_selectedEntities.clear();
						
						IterateEntities(*this, updateInfo.m_viewInfo, [&] (Entity& entity, const Rectangle& iconRect)
						{
							if (iconRect.Intersects(m_selectionRect) && !IsEntitySelected(entity))
								m_selectedEntities.emplace_back(entity);
						});
					}
				}
			}
			else
			{
				if (frameBeginTime <= m_timeOfLastMouseRelease + 0.4 &&
					LengthSquared(m_positionOfLastMouseRelease - updateInfo.m_mouse.pos) < 5 * 5)
				{
					Entity* propertiesEntity = nullptr;
					
					IterateEntities(*this, updateInfo.m_viewInfo, [&] (Entity& entity, const Rectangle& iconRect)
					{
						if (iconRect.Contains(updateInfo.m_mouse.pos))
							propertiesEntity = &entity;
					});
					
					if (propertiesEntity != nullptr)
					{
						GetEditor().ShowPropertiesWindow(*propertiesEntity);
					}
				}
				else
				{
					m_timeOfLastMouseRelease = frameBeginTime;
					m_positionOfLastMouseRelease = updateInfo.m_mouse.pos;
				}
				
				if (!m_selectedEntities.empty() && m_moveIconRectangle.Contains(updateInfo.m_mouse.pos))
				{
					BeginMovingSelected(mousePositionWS);
				}
				else
				{
					m_selectionBegin = updateInfo.m_mouse.pos;
					m_selectionRectOpacity = 1.0f;
					m_isSelecting = false;
					
					if (!updateInfo.m_keyboard.IsAnyDown(KEY_MASK_CONTROL))
					{
						m_selectedEntities.clear();
						m_pathEditEntity = nullptr;
					}
					
					Entity* clickedEntity = nullptr;
					
					IterateEntities(*this, updateInfo.m_viewInfo, [&] (Entity& entity, const Rectangle& iconRect)
					{
						if (iconRect.Contains(updateInfo.m_mouse.pos))
							clickedEntity = &entity;
					});
					
					if (clickedEntity != nullptr && !IsEntitySelected(*clickedEntity))
						m_selectedEntities.emplace_back(*clickedEntity);
					
					if (m_selectedEntities.size() == 1)
						BeginMovingSelected(mousePositionWS);
				}
			}
		}
		else
		{
			m_isMovingSelected = false;
			
			if (m_selectionRectOpacity > 0.0f)
			{
				m_selectionRectOpacity -= updateInfo.m_dt * 10;
				if (m_selectionRectOpacity < 0.0f)
				{
					m_selectionRectOpacity = 0.0f;
					m_isSelecting = false;
				}
			}
		}
		
		if (updateInfo.m_keyboard.IsDown(Key::X) && !updateInfo.m_keyboard.WasDown(Key::X))
			DespawnSelected();
		
		if (!updateInfo.m_keyboard.IsAnyDown(KEY_MASK_CONTROL) && updateInfo.m_keyboard.IsDown(Key::C) &&
		    !updateInfo.m_keyboard.WasDown(Key::C) && !m_isMovingSelected)
		{
			std::vector<SelectedEntity> newSelection;
			
			for (size_t i = 0; i < m_selectedEntities.size(); i++)
			{
				std::unique_ptr<Entity> clone = m_selectedEntities[i].m_entity->Clone();
				
				if (clone != nullptr)
				{
					clone->GetTransform().Translate(glm::vec2(0.3f));
					newSelection.emplace_back(*clone);
					GetGameWorld().Spawn(std::move(clone));
				}
			}
			
			m_selectedEntities = newSelection;
			m_pathEditEntity = nullptr;
		}
		
		if (m_showSpawnMenuNextUpdate ||
			(!updateInfo.m_keyboard.IsAnyDown(KEY_MASK_CONTROL) && updateInfo.m_keyboard.IsDown(Key::S) &&
		     !updateInfo.m_keyboard.WasDown(Key::S) && !m_isMovingSelected))
		{
			m_spawnMenu.Show(mousePositionWS);
			m_selectedEntities.clear();
			m_pathEditEntity = nullptr;
			m_showSpawnMenuNextUpdate = false;
		}
		
		m_wasMouseLeftPressed = updateInfo.m_mouse.IsDown(MouseButton::Left);
	}
	
	void EntityTool::BeginMovingSelected(glm::vec2 mouseCoordsWS)
	{
		m_isMovingSelected = true;
		for (SelectedEntity& entity : m_selectedEntities)
			entity.m_moveBeginLocation = entity.m_entity->GetTransform().GetPosition();
		m_moveBeginMousePosWs = mouseCoordsWS;
	}
	
	extern std::vector<glm::ivec2> lastPath;
	extern std::vector<std::array<glm::vec2, 2>> lastPathPortals;
	
	void EntityTool::DrawUI(const ViewInfo& viewInfo, UIRenderer& uiRenderer)
	{
		m_spawnMenu.Render();
		
		// ** Draws the selection box **
		const glm::vec4 DEFAULT_SHADE = { 1.0f, 1.0f, 1.0f, 1.0f };
		const glm::vec4 SELECTION_SHADE = { 67 / 255.0f, 183 / 255.0f, 222 / 255.0f, 1.0f };
		
		if (m_isSelecting && m_selectionRectOpacity > 0.0f)
		{
			uiRenderer.DrawRectangle(m_selectionRect, glm::vec4(glm::vec3(SELECTION_SHADE), 0.6f * m_selectionRectOpacity));
		}
		
		// ** Draws per entity icons **
		if (m_drawEntityInterfaces || m_drawEntityIcons)
		{
			glm::mat3 viewMatrix = glm::mat3(
					GetWindowWidth() / 2.0f, 0.0f, 0.0f,
					0.0f, GetWindowHeight() / 2.0f, 0.0f,
					GetWindowWidth() / 2.0f, GetWindowHeight() / 2.0f, 1.0f
			) * viewInfo.GetViewMatrix();
			
			IterateEntities(*this, viewInfo, [&] (Entity& entity, const Rectangle& iconRect)
			{
				if (m_drawEntityInterfaces)
				{
					if (const IEditorUIEntity* uiEntity = dynamic_cast<const IEditorUIEntity*>(&entity))
						uiEntity->DrawEditorUI(uiRenderer, viewMatrix);
					
					if (!entity.GetName().empty())
					{
						const Font& font = Font::GetNamedFont(FontNames::StandardUI);
						
						uiRenderer.DrawString(font, entity.GetName(), { iconRect.CenterX(), iconRect.y - 5, 0, 0 },
						                      Alignment::Center, Alignment::Top, glm::vec4(1.0f));
					}
				}
				
				if (m_drawEntityIcons)
				{
					glm::vec4 shade = IsEntitySelected(entity) ? SELECTION_SHADE : DEFAULT_SHADE;
					if (&entity == GetEditor().GetFocusedPropertiesObject())
						shade = glm::vec4(glm::vec3(shade) * 1.3f, 1.0f);
					uiRenderer.DrawSprite(m_entityIcon, iconRect, glm::convertSRGBToLinear(shade));
				}
			});
		}
		
		// ** Draws the move icon **
		if (!m_selectedEntities.empty())
		{
			uiRenderer.DrawSprite(m_moveIcon, m_moveIconRectangle, glm::vec4(1.0f));
		}
	}
	
	void EntityTool::SetGameWorld(GameWorld& gameWorld)
	{
		m_spawnMenu.SetGameWorld(&gameWorld);
		EditorTool::SetGameWorld(gameWorld);
	}
	
	bool EntityTool::OnContextMenuOpen(const ViewInfo& viewInfo, glm::vec2 screenCursorPos, glm::vec2)
	{
		m_contextMenuEntity = nullptr;
		
		IterateEntities(*this, viewInfo, [&] (Entity& entity, const Rectangle& iconRect)
		{
			if (iconRect.Contains(screenCursorPos))
				m_contextMenuEntity = &entity;
		});
		
		if (m_contextMenuEntity != nullptr)
		{
			m_selectedEntities.clear();
			m_selectedEntities.emplace_back(*m_contextMenuEntity);
			
			if (auto editablePathProvider = dynamic_cast<const IEditablePathProvider*>(m_contextMenuEntity))
				m_contextMenuEditPathLabel = std::string("Edit ") + editablePathProvider->GetEditPathName();
			else
				m_contextMenuEditPathLabel.clear();
		}
		
		return true;
	}
	
	void EntityTool::RenderContextMenu()
	{
		if (ImGui::MenuItem("Spawn Entity", "S"))
			m_showSpawnMenuNextUpdate = true;
		
		if (m_contextMenuEntity == nullptr)
			return;
		
		ImGui::Separator();
		
		if (!m_contextMenuEditPathLabel.empty() && ImGui::MenuItem(m_contextMenuEditPathLabel.c_str()))
		{
			GetEditor().EditPath(dynamic_cast<IEditablePathProvider&>(*m_contextMenuEntity));
		}
		
		if (ImGui::MenuItem("Despawn", "X"))
		{
			m_contextMenuEntity->Despawn();
			
			//Removes the entity from the list of selected entities
			std::remove_if(m_selectedEntities.begin(), m_selectedEntities.end(), [&] (const SelectedEntity& s)
			{
				return s.m_entity == m_contextMenuEntity;
			});
			
			m_pathEditEntity = nullptr;
		}
		
		if (ImGui::MenuItem("Clone", "C"))
		{
			std::unique_ptr<Entity> clone = m_contextMenuEntity->Clone();
			
			if (clone != nullptr)
			{
				clone->GetTransform().Translate(glm::vec2(0.3f));
				
				m_selectedEntities.clear();
				m_selectedEntities.emplace_back(*clone);
				m_pathEditEntity = nullptr;
				
				GetGameWorld().Spawn(std::move(clone));
			}
		}
	}
	
	void EntityTool::RenderViewMenu()
	{
		ImGui::Separator();
		
		ImGui::MenuItem("Entity Icons", nullptr, &m_drawEntityIcons);
		ImGui::MenuItem("Entity Interfaces", nullptr, &m_drawEntityInterfaces);
	}
	
	void EntityTool::DespawnSelected()
	{
		for (const SelectedEntity& selectedEntity : m_selectedEntities)
			selectedEntity.m_entity->Despawn();
		m_selectedEntities.clear();
		m_pathEditEntity = nullptr;
	}
	
	bool EntityTool::IsEntitySelected(const Entity& entity) const
	{
		return std::find_if(m_selectedEntities.begin(), m_selectedEntities.end(), [&] (const SelectedEntity& e)
		{
			return e.m_entity == &entity;
		}) != m_selectedEntities.end();
	}
	
	EntityTool::SelectedEntity::SelectedEntity(Entity& entity)
		: m_entity(&entity), m_moveBeginLocation(entity.GetTransform().GetPosition()) { }
}
