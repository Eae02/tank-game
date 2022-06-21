#include "vertexinputstate.h"

#include <tuple>

namespace TankGame
{
	static uint32_t currentID = 0;
	static uint32_t nextID = 1;
	
	VertexInputState::VertexInputState()
		: m_id(nextID++)
	{
		GLuint vao;
		glGenVertexArrays(1, &vao);
		m_vao.SetID(vao);
	}
	
	static std::tuple<int, GLenum, bool> vertexAttribFormats[] = 
	{
		/* Float32_1 */ { 1, GL_FLOAT, false },
		/* Float32_2 */ { 2, GL_FLOAT, false },
		/* Float32_3 */ { 3, GL_FLOAT, false },
		/* Float32_4 */ { 4, GL_FLOAT, false },
		/* UNorm8_1  */ { 1, GL_UNSIGNED_BYTE, true },
		/* UNorm8_2  */ { 2, GL_UNSIGNED_BYTE, true },
		/* UNorm8_3  */ { 3, GL_UNSIGNED_BYTE, true },
		/* UNorm8_4  */ { 4, GL_UNSIGNED_BYTE, true },
		/* UInt8_1   */ { 1, GL_UNSIGNED_BYTE, false },
		/* UInt8_2   */ { 2, GL_UNSIGNED_BYTE, false },
		/* UInt8_3   */ { 3, GL_UNSIGNED_BYTE, false },
		/* UInt8_4   */ { 4, GL_UNSIGNED_BYTE, false },
		/* Int16_1   */ { 1, GL_SHORT, false },
		/* Int16_2   */ { 2, GL_SHORT, false },
		/* Int16_3   */ { 3, GL_SHORT, false },
		/* Int16_4   */ { 4, GL_SHORT, false },
	};
	
	void VertexInputState::UpdateAttribute(uint32_t index, GLuint buffer, VertexAttribFormat format, size_t offset, size_t stride)
	{
		Bind();
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		auto [components, type, normalized] = vertexAttribFormats[(int)format];
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(
			index, components, type, normalized, stride,
			reinterpret_cast<void*>(static_cast<uintptr_t>(offset)));
	}
	
	void VertexInputState::SetAttributeInstanceStep(uint32_t index, bool instanceStep)
	{
		Bind();
		glEnableVertexAttribArray(index);
		glVertexAttribDivisor(index, static_cast<GLuint>(instanceStep));
	}
	
	void VertexInputState::Bind() const
	{
		if (currentID != m_id)
		{
			currentID = m_id;
			glBindVertexArray(m_vao.GetID());
		}
	}
	
	static GLuint emptyVAO;
	
	void VertexInputState::BindEmpty()
	{
		if (currentID != 0)
		{
			currentID = 0;
			glBindVertexArray(emptyVAO);
		}
	}
	
	void VertexInputState::InitEmpty()
	{
		glGenVertexArrays(1, &emptyVAO);
		glBindVertexArray(emptyVAO);
	}
	
	void VertexInputState::DeleteVertexArray(GLuint id)
	{
		glDeleteVertexArrays(1, &id);
	}
}
