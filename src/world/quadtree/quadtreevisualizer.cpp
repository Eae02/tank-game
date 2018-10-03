#include "quadtreevisualizer.h"
#include "quadtree.h"
#include "../../transform.h"

namespace TankGame
{
	void QuadTreeVisualizer::DrawQuadTree(UIRenderer& uiRenderer, const QuadTree& quadTree,
	                                      const glm::mat3& viewMatrix)
	{
		DrawQuadTreeNode(uiRenderer, quadTree.m_rootNode, viewMatrix);
	}
	
	void QuadTreeVisualizer::DrawQuadTreeNode(UIRenderer& uiRenderer, const class QuadTreeNode& node,
	                                          const glm::mat3& viewMatrix)
	{
		if (node.m_isLeaf)
			return;
		
		glm::vec2 toEdge[2] = { { node.m_area.w / 2.0f, 0.0f }, { 0.0f, node.m_area.h / 2.0f } };
		
		for (int i = 0; i < 2; i++)
		{
			glm::vec2 vertex1 = node.m_area.Center() + toEdge[i];
			glm::vec2 vertex2 = node.m_area.Center() - toEdge[i];
			
			uiRenderer.DrawLine(glm::vec2(viewMatrix * glm::vec3(vertex1, 1.0f)),
			                    glm::vec2(viewMatrix * glm::vec3(vertex2, 1.0f)), { 1, 1, 1, 1 });
		}
		
		for (int i = 0; i < 4; i++)
			DrawQuadTreeNode(uiRenderer, *node.m_children[i], viewMatrix);
	}
}
