#pragma once

#include "../utils/abstract.h"
#include <glm/glm.hpp>

namespace TankGame
{
	//Interface for entities with special editor UI (e.g. event boxes)
	class IEditorUIEntity : public Abstract
	{
	public:
		virtual void DrawEditorUI(class UIRenderer& uiRenderer, const glm::mat3& uiViewMatrix) const = 0;
	};
}
