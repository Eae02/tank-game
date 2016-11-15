#include "renderbuffer.h"

namespace TankGame
{
	Renderbuffer::Renderbuffer(GLsizei width, GLsizei height, GLenum internalFormat)
	{
		GLuint renderbuffer;
		glCreateRenderbuffers(1, &renderbuffer);
		SetID(renderbuffer);
		
		glNamedRenderbufferStorage(renderbuffer, internalFormat, width, height);
	}
	
	void DeleteRenderBuffer(GLuint id)
	{
		glDeleteRenderbuffers(1, &id);
	}
}
