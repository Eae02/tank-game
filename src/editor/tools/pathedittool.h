#pragma once

#include "editortool.h"

namespace TankGame
{
	class PathEditTool : public EditorTool
	{
	public:
		explicit PathEditTool(class Editor& editor);
		
		virtual void Update(const UpdateInfo& updateInfo) override;
		virtual void DrawUI(const class ViewInfo& viewInfo, class UIRenderer& uiRenderer) override;
		
		virtual bool OnContextMenuOpen(const class ViewInfo&, glm::vec2, glm::vec2) override;
		
		void SetPathProvider(class IEditablePathProvider* pathProvider);
		
		void StopEditing();
		
	private:
		long PickNode(const glm::mat3& viewMatrix, glm::vec2 screenCoordinate) const;
		
		Texture2D m_pathPoint;
		Texture2D m_pathArrow;
		
		long m_currentMovingNode = -1;
		glm::vec2 m_moveVector;
		glm::vec2 m_moveBeginPosition;
		
		long m_lineSubdivideIndex = -1;
		
		bool m_insertAtEnd = true;
		
		bool m_isClosed;
		
		class IEditablePathProvider* m_pathProvider = nullptr;
	};
}
