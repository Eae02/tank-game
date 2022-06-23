#pragma once

#include "uirenderer.h"
#include <glm/glm.hpp>

namespace TankGame
{
	void DrawButton(const std::string& text, UIRenderer& uiRenderer, glm::vec2 pos, float height, float opacity = 1.0f);
}
