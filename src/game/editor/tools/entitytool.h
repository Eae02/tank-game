#pragma once

#include "editortool.h"
#include "entityspawnmenu.h"
#include "../../updateinfo.h"
#include "../../graphics/viewinfo.h"
#include "../../graphics/ui/uirenderer.h"
#include "../../world/path/path.h"

#include <vector>

namespace TankGame
{
	class EntityTool : public EditorTool
	{
		template <typename CallbackTp>
		friend void IterateEntities(EntityTool& self, const ViewInfo& viewInfo, CallbackTp callback);
		
	public:
		explicit EntityTool(class Editor& editor);
		
		virtual void Update(const UpdateInfo& updateInfo) override;
		virtual void DrawUI(const ViewInfo& viewInfo, class UIRenderer& uiRenderer) override;
		
		virtual void SetGameWorld(class GameWorld& gameWorld) override;
		
		virtual bool OnContextMenuOpen(const class ViewInfo& viewInfo, glm::vec2 screenCursorPos, glm::vec2) override;
		virtual void RenderContextMenu() override;
		
		virtual void RenderViewMenu() override;
		
		void DespawnSelected();
		
	private:
		void BeginMovingSelected(glm::vec2 mouseCoordsWS);
		bool IsEntitySelected(const Entity& entity) const;
		
		Rectangle GetIconRectangle(const ViewInfo& viewInfo, Entity& entity);
		
		std::string m_contextMenuEditPathLabel;
		Entity* m_contextMenuEntity;
		
		Texture2D m_entityIcon;
		Texture2D m_moveIcon;
		
		EntitySpawnMenu m_spawnMenu;
		
		bool m_showSpawnMenuNextUpdate = false;
		
		bool m_drawEntityIcons = true;
		bool m_drawEntityInterfaces = true;
		
		double m_timeOfLastMouseRelease = 0.0;
		glm::vec2 m_positionOfLastMouseRelease;
		
		glm::vec2 m_moveBeginMousePosWs;
		
		struct SelectedEntity
		{
			Entity* m_entity;
			glm::vec2 m_moveBeginLocation;
			
			explicit SelectedEntity(Entity& entity);
		};
		
		class IPathEditableEntity* m_pathEditEntity = nullptr;
		
		std::vector<SelectedEntity> m_selectedEntities;
		bool m_wasMouseLeftPressed = false;
		
		glm::vec2 m_selectionBegin;
		Rectangle m_selectionRect;
		bool m_isSelecting = false;
		float m_selectionRectOpacity = 0.0f;
		
		Rectangle m_moveIconRectangle;
		bool m_isMovingSelected = false;
	};
}
