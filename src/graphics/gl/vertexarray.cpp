#include "vertexarray.h"

namespace TankGame
{
	GLuint VertexArray::s_currentVAO = 0;
	
	VertexArray::VertexArray()
	{
		GLuint vao;
		glCreateVertexArrays(1, &vao);
		SetID(vao);
	}
	
	void VertexArray::Bind() const
	{
		if (s_currentVAO == GetID())
			return;
		s_currentVAO = GetID();
		glBindVertexArray(GetID());
	}
}
