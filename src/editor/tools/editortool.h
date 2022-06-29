#pragma once


#include "../../updateinfo.h"
#include "../../graphics/gl/texture2d.h"
#include "../../utils/abstract.h"

namespace TankGame
{
	class EditorTool : public Abstract
	{
	public:
		inline explicit EditorTool(class Editor& editor)
		    : m_editor(editor) { }
		
		virtual void Update(const class UpdateInfo& updateInfo) = 0;
		virtual void DrawUI(const class ViewInfo& viewInfo, class UIRenderer& uiRenderer) = 0;
		
		virtual void SetGameWorld(class GameWorld& gameWorld);
		virtual void OnResize(int newWidth, int newHeight);
		
		virtual bool OnContextMenuOpen(const class ViewInfo& viewInfo, glm::vec2 screenCursorPos, glm::vec2 worldCursorPos)
		{ return true; }
		virtual void RenderContextMenu() { }
		
		virtual void RenderViewMenu() { }
		
		glm::vec2 GetOldWorldMouseCoords(const class UpdateInfo& updateInfo);
		glm::vec2 GetNewWorldMouseCoords(const class UpdateInfo& updateInfo);
		
	protected:
		glm::mat3 GetScreenSpaceViewMatrix(const class ViewInfo& viewInfo) const;
		
		inline class GameWorld& GetGameWorld()
		{ return *m_gameWorld; }
		
		inline class Editor& GetEditor()
		{ return m_editor; }
		
		inline int GetWindowWidth() const
		{ return m_windowWidth; }
		inline int GetWindowHeight() const
		{ return m_windowHeight; }
		
	private:
		class GameWorld* m_gameWorld;
		class Editor& m_editor;
		
		int m_windowWidth, m_windowHeight;
	};
}
