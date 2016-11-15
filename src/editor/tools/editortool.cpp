#include "editortool.h"
#include "../../graphics/viewinfo.h"
#include "../../updateinfo.h"
#include "../../mouse.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"

namespace TankGame
{
	void EditorTool::SetGameWorld(class GameWorld& gameWorld)
	{
		m_gameWorld = &gameWorld;
	}
	
	void EditorTool::OnResize(int newWidth, int newHeight)
	{
		m_windowWidth = newWidth;
		m_windowHeight = newHeight;
	}
	
	glm::vec2 EditorTool::GetOldWorldMouseCoords(const UpdateInfo& updateInfo)
	{
		glm::vec2 screenSpacePos = updateInfo.m_mouse.GetOldPosition() / glm::vec2(m_windowWidth, m_windowHeight);
		return updateInfo.m_viewInfo.ScreenToWorld(screenSpacePos);
	}
	
	glm::vec2 EditorTool::GetNewWorldMouseCoords(const UpdateInfo& updateInfo)
	{
		glm::vec2 screenSpacePos = updateInfo.m_mouse.GetPosition() / glm::vec2(m_windowWidth, m_windowHeight);
		return updateInfo.m_viewInfo.ScreenToWorld(screenSpacePos);
	}
	
	glm::mat3 EditorTool::GetScreenSpaceViewMatrix(const ViewInfo& viewInfo) const
	{
		return glm::mat3(
				m_windowWidth / 2.0f, 0.0f, 0.0f,
				0.0f, m_windowHeight / 2.0f, 0.0f,
				m_windowWidth / 2.0f, m_windowHeight / 2.0f, 1.0f
		) * viewInfo.GetViewMatrix();
	}
}
