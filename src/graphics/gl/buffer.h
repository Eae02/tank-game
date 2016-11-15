#pragma once

#include "glresource.h"

namespace TankGame
{
	void DeleteBuffer(GLuint id);
	
	class Buffer : public GLResource<DeleteBuffer>
	{
	public:
		inline Buffer(size_t size, GLbitfield flags)
		    : Buffer(size, nullptr, flags) { }
		
		Buffer(size_t size, const void* data, GLbitfield flags);
		
	private:
		
	};
}
