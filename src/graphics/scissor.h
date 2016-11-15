#pragma once

#include "../rectangle.h"

#include <GL/glew.h>

namespace TankGame
{
	void PushScissorRect(const Rectangle& rectangle);
	void PushScissorRect(GLint x, GLint y, GLsizei w, GLsizei h);
	void PopScissorRect();
}
