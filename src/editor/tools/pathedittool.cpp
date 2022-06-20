#include "pathedittool.h"
#include "../ieditablepathprovider.h"
#include "../../graphics/ui/uirenderer.h"
#include "../../graphics/viewinfo.h"
#include "../../utils/ioutils.h"
#include "../../utils/mathutils.h"
#include "../../world/path/path.h"
#include "../../mouse.h"
#include "../../keyboard.h"

#include <cmath>

namespace TankGame
{
	static const float PATH_POINT_WIDTH = 32;
	static const float PATH_ARROW_WIDTH = 32;
	
	PathEditTool::PathEditTool(class Editor& editor)
	    : EditorTool(editor), m_pathPoint(Texture2D::FromFile(resDirectoryPath / "ui" / "path-point.png")),
	      m_pathArrow(Texture2D::FromFile(resDirectoryPath / "ui" / "path-arrow.png"))
	{
		
	}
	
	long PathEditTool::PickNode(const glm::mat3& viewMatrix, glm::vec2 screenCoordinate) const
	{
		const Path& path = m_pathProvider->GetEditPath();
		
		long beginNode = path.GetNodeCount() - (m_isClosed ? 2 : 1);
		
		for (long i = beginNode; i >= 0; i--)
		{
			const float MAX_DIST_SQ = PATH_ARROW_WIDTH * PATH_ARROW_WIDTH * 0.25f;
			
			glm::vec2 nodePosSS(viewMatrix * glm::vec3(path[i], 1.0f));
			
			if (LengthSquared(nodePosSS - screenCoordinate) < MAX_DIST_SQ)
				return i;
		}
		
		return -1;
	}
	
	bool PathEditTool::OnContextMenuOpen(const ViewInfo&, glm::vec2, glm::vec2)
	{
		return false;
	}
	
	void PathEditTool::Update(const UpdateInfo& updateInfo)
	{
		if (m_pathProvider == nullptr)
			return;
		
		glm::mat3 ssViewMatrix = GetScreenSpaceViewMatrix(updateInfo.m_viewInfo);
		glm::vec2 worldMouseCoords = GetNewWorldMouseCoords(updateInfo);
		
		Path& path = m_pathProvider->GetEditPath();
		
		if (m_currentMovingNode == -1 && path.GetNodeCount() > 3 &&
			updateInfo.m_mouse.IsDown(MouseButton::Right) && !updateInfo.m_mouse.WasDown(MouseButton::Right))
		{
			long nodeIndex = PickNode(ssViewMatrix, updateInfo.m_mouse.pos);
			if (nodeIndex != -1)
				path.RemoveNodes(nodeIndex, 1);
		}
		
		auto LockToGridIfShiftPressed = [&] (glm::vec2& position)
		{
			if (updateInfo.m_keyboard.IsAnyDown(KEY_MASK_SHIFT))
			{
				position.x = std::round(position.x * 2.0f) / 2.0f;
				position.y = std::round(position.y * 2.0f) / 2.0f;
			}
		};
		
		if (updateInfo.m_mouse.IsDown(MouseButton::Left))
		{
			if (!updateInfo.m_mouse.WasDown(MouseButton::Left))
			{
				m_currentMovingNode = PickNode(ssViewMatrix, updateInfo.m_mouse.pos);
				m_moveVector = glm::vec2(0.0f);
				
				if (m_currentMovingNode != -1)
				{
					m_moveBeginPosition = path[m_currentMovingNode];
				}
				else if (m_lineSubdivideIndex != -1)
				{
					glm::vec2 newNodePos = (path[m_lineSubdivideIndex] + path[m_lineSubdivideIndex - 1]) / 2.0f;
					path.InsertNode(newNodePos, m_lineSubdivideIndex);
				}
				else if (!m_isClosed)
				{
					glm::vec2 newNodePos = worldMouseCoords;
					LockToGridIfShiftPressed(newNodePos);
					
					if (m_insertAtEnd)
						path.AddNode(newNodePos);
					else
						path.InsertNode(newNodePos, 0);
				}
			}
			else if (m_currentMovingNode != -1)
			{
				m_moveVector += worldMouseCoords - GetOldWorldMouseCoords(updateInfo);
				
				path[m_currentMovingNode] = m_moveBeginPosition + m_moveVector;
				
				LockToGridIfShiftPressed(path[m_currentMovingNode]);
				
				if (m_isClosed && m_currentMovingNode == 0)
					path[path.GetNodeCount() - 1] = path[0];
			}
		}
		else
			m_currentMovingNode = -1;
		
		m_lineSubdivideIndex = -1;
		float closestCenterDistSq = std::pow(1, 2);
		
		for (long i = path.GetNodeCount() - 1; i >= 1; i--)
		{
			glm::vec2 center = (path[i] + path[i - 1]) / 2.0f;
			
			float maxDistSq = LengthSquared(center - path[i]) * 0.8f;
			float distToCenterSq = LengthSquared(center - worldMouseCoords);
			
			if (distToCenterSq < closestCenterDistSq && distToCenterSq < maxDistSq)
			{
				m_lineSubdivideIndex = i;
				closestCenterDistSq = distToCenterSq;
			}
		}
	}
	
	void PathEditTool::DrawUI(const ViewInfo& viewInfo, UIRenderer& uiRenderer)
	{
		if (m_pathProvider == nullptr)
			return;
		
		glm::mat3 viewMatrix = GetScreenSpaceViewMatrix(viewInfo);
		
		const Path& path = m_pathProvider->GetEditPath();
		
		for (long i = path.GetNodeCount() - 1; i >= 0; i--)
		{
			glm::vec2 lineEnd(viewMatrix * glm::vec3(path[i], 1));
			
			if (i != 0)
			{
				glm::vec2 lineBegin(viewMatrix * glm::vec3(path[i - 1], 1));
				
				uiRenderer.DrawLine(lineEnd, lineBegin, glm::vec4(1.0f));
				
				if (m_lineSubdivideIndex == i && m_currentMovingNode == -1)
				{
					glm::vec2 centerLine = (lineBegin + lineEnd) / 2.0f;
					Rectangle iconRect = Rectangle::CreateCentered(centerLine, PATH_POINT_WIDTH / 2.0f, PATH_POINT_WIDTH / 2.0f);
					uiRenderer.DrawSprite(m_pathPoint, iconRect, glm::vec4(ParseColorHexCodeSRGB(0xFA953C), 1.0f));
				}
			}
			
			if (!m_isClosed || i != 0)
			{
				Rectangle iconRect = Rectangle::CreateCentered(lineEnd, PATH_POINT_WIDTH / 2.0f, PATH_POINT_WIDTH / 2.0f);
				uiRenderer.DrawSprite(m_pathPoint, iconRect, glm::vec4(1.0f));
			}
		}
	}
	
	void PathEditTool::SetPathProvider(IEditablePathProvider* pathProvider)
	{
		m_pathProvider = pathProvider;
		m_isClosed = pathProvider->IsClosedPath();
		m_currentMovingNode = -1;
	}
	
	void PathEditTool::StopEditing()
	{
		m_pathProvider->PathEditEnd();
	}
}
