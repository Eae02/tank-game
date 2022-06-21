#pragma once

#include "gl/glresource.h"
#include "gl/buffer.h"
#include "gl/shadermodule.h"
#include "gl/vertexinputstate.h"

#include <memory>

namespace TankGame
{
	class QuadMesh
	{
	public:
		QuadMesh();
		
		inline GLuint GetVBO() const
		{ return m_vbo.GetID(); }
		
		inline void BindVAO() const
		{ m_vertexInputState.Bind(); }
		
		static inline void SetInstance(std::unique_ptr<QuadMesh>&& instance)
		{ s_instance = std::move(instance); }
		static inline QuadMesh& GetInstance()
		{ return *s_instance; }
		
		void InitializeVertexAttribute(VertexInputState& vertexInputState, uint32_t attribIndex);
		
		static const ShaderModule& GetVertexShader();
		
	private:
		static std::unique_ptr<QuadMesh> s_instance;
		static std::unique_ptr<ShaderModule> s_quadVertexShaderModule;
		
		Buffer m_vbo;
		VertexInputState m_vertexInputState;
	};
}
