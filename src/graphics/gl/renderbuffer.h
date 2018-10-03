#pragma once

#include "glresource.h"

namespace TankGame
{
	void DeleteRenderBuffer(GLuint id);
	
	class Renderbuffer : public GLResource<DeleteRenderBuffer>
	{
	public:
		Renderbuffer(GLsizei width, GLsizei height, GLenum internalFormat);
	};
}
