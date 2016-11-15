#include "buffer.h"

namespace TankGame
{
	Buffer::Buffer(size_t size, const void* data, GLbitfield flags)
	{
		GLuint buffer;
		glCreateBuffers(1, &buffer);
		SetID(buffer);
		
		glNamedBufferStorage(buffer, size, data, flags);
	}
	
	void DeleteBuffer(GLuint id)
	{
		glDeleteBuffers(1, &id);
	}
}
