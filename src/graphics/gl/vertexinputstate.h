#pragma once

#include "functions.h"
#include "glresource.h"

namespace TankGame
{
	enum class VertexAttribFormat
	{
		Float32_1,
		Float32_2,
		Float32_3,
		Float32_4,
		UNorm8_1,
		UNorm8_2,
		UNorm8_3,
		UNorm8_4,
		UInt8_1,
		UInt8_2,
		UInt8_3,
		UInt8_4,
		Int16_1,
		Int16_2,
		Int16_3,
		Int16_4,
	};
	
	class VertexInputState
	{
	public:
		VertexInputState();
		
		void UpdateAttribute(uint32_t index, GLuint buffer, VertexAttribFormat format, size_t offset, size_t stride);
		
		void SetAttributeInstanceStep(uint32_t index, bool instanceStep = true);
		
		void Bind() const;
		
		static void BindEmpty();
		static void InitEmpty();
		
	private:
		static void DeleteVertexArray(GLuint id);
		
		uint32_t m_id;
		
		GLResource<DeleteVertexArray> m_vao;
	};
}
