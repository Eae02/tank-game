#pragma once

#include "../../graphics/ui/uirenderer.h"

namespace TankGame
{
	class QuadTreeVisualizer
	{
	public:
		void DrawQuadTree(UIRenderer& uiRenderer, const class QuadTree& quadTree, const glm::mat3& viewMatrix);
		
	private:
		void DrawQuadTreeNode(UIRenderer& uiRenderer, const class QuadTreeNode& node, const glm::mat3& viewMatrix);
	};
}
