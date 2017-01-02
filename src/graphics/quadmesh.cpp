#include "quadmesh.h"
#include "../utils/ioutils.h"
#include "../utils/utils.h"

namespace TankGame
{
	std::unique_ptr<QuadMesh> QuadMesh::s_instance;
	std::unique_ptr<ShaderModule> QuadMesh::s_quadVertexShaderModule;
	
	const ShaderModule& QuadMesh::GetVertexShader()
	{
		if (s_quadVertexShaderModule == nullptr)
		{
			const fs::path vsPath = GetResDirectory() / "shaders" / "fullscreenquad.vs.glsl";
			
			s_quadVertexShaderModule = std::make_unique<ShaderModule>(ShaderModule::FromFile(vsPath, GL_VERTEX_SHADER));
			CallOnClose([] { s_quadVertexShaderModule = nullptr; });
		}
		
		return *s_quadVertexShaderModule;
	}
	
	static const float vertices[] =
	{
		-1.0f, -1.0f,
		-1.0f,  1.0f,
		1.0f, -1.0f,
		1.0f,  1.0f
	};
	
	QuadMesh::QuadMesh() : m_vbo(sizeof(vertices), vertices, 0)
	{
		glEnableVertexArrayAttrib(m_vertexArray.GetID(), 0);
		glVertexArrayVertexBuffer(m_vertexArray.GetID(), 0, m_vbo.GetID(), 0, sizeof(float) * 2);
		glVertexArrayAttribFormat(m_vertexArray.GetID(), 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(m_vertexArray.GetID(), 0, 0);
	}
}
