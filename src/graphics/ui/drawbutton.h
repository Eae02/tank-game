#pragma once

#include "uirenderer.h"
#include <glm/glm.hpp>

namespace TankGame
{
	void DrawButton(int button, UIRenderer& uiRenderer, glm::vec2 pos, float height, float opacity = 1.0f);
}
