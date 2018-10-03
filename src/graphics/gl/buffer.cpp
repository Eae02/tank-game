#include "buffer.h"

#include <algorithm>

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
	
	static size_t uniformBufferOffsetAlignment = 0;
	
	size_t GetUniformBufferOffsetAlignment()
	{
		if (uniformBufferOffsetAlignment == 0)
		{
			GLint value;
			glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &value);
			uniformBufferOffsetAlignment = static_cast<size_t>(std::max(value, 0));
		}
		
		return uniformBufferOffsetAlignment;
	}
}
