#include "editorrenderer.h"
#include "../graphics/viewinfo.h"

namespace TankGame
{
	void EditorRenderer::DrawLighting(const ViewInfo& viewInfo) const
	{
		if (!m_useGameLighting)
		{
			const float BASE_LIGHTING = 0.5f;
			float clearColor[] = { BASE_LIGHTING, BASE_LIGHTING, BASE_LIGHTING, BASE_LIGHTING };
			glClearBufferfv(GL_COLOR, 0, clearColor);
		}
		
		WorldRenderer::DrawLighting(viewInfo);
	}
}
