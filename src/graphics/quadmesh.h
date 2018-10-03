#pragma once

#include "gl/glresource.h"
#include "gl/vertexarray.h"
#include "gl/buffer.h"
#include "gl/shadermodule.h"

#include <memory>

namespace TankGame
{
	class QuadMesh
	{
	public:
		QuadMesh();
		
		inline GLuint GetVBO() const
		{ return m_vbo.GetID(); }
		
		inline const VertexArray& GetVAO() const
		{ return m_vertexArray; }
		
		static inline void SetInstance(std::unique_ptr<QuadMesh>&& instance)
		{ s_instance = std::move(instance); }
		static inline QuadMesh& GetInstance()
		{ return *s_instance; }
		
		static const ShaderModule& GetVertexShader();
		
	private:
		static std::unique_ptr<QuadMesh> s_instance;
		static std::unique_ptr<ShaderModule> s_quadVertexShaderModule;
		
		Buffer m_vbo;
		VertexArray m_vertexArray;
	};
}
