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
			const fs::path vsPath = resDirectoryPath / "shaders" / "fullscreenquad.vs.glsl";
			
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
	
	QuadMesh::QuadMesh()
		: m_vbo(sizeof(vertices), vertices, BufferUsage::StaticVertex)
	{
		InitializeVertexAttribute(m_vertexInputState, 0);
	}
	
	void QuadMesh::InitializeVertexAttribute(VertexInputState& vertexInputState, uint32_t attribIndex)
	{
		vertexInputState.UpdateAttribute(attribIndex, m_vbo.GetID(), VertexAttribFormat::Float32_2, 0, sizeof(float) * 2);
	}
}
