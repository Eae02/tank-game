#pragma once

#include "glresource.h"

namespace TankGame
{
	inline void DeleteVertexArray(GLuint id)
	{
		glDeleteVertexArrays(1, &id);
	}
	
	class VertexArray : public GLResource<DeleteVertexArray>
	{
	public:
		VertexArray();
		
		void Bind() const;
		
	private:
		static GLuint s_currentVAO;
	};
}
