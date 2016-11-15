#include "quadmesh.h"
#include "../utils/ioutils.h"
#include "../utils/utils.h"

namespace TankGame
{
	std::unique_ptr<QuadMesh> QuadMesh::s_instance;
	StackObject<ShaderModule> QuadMesh::s_quadVertexShaderModule;
	
	const ShaderModule& QuadMesh::GetVertexShader()
	{
		if (s_quadVertexShaderModule.IsNull())
		{
			s_quadVertexShaderModule.Construct(
				ShaderModule::FromFile(GetResDirectory() / "shaders" / "fullscreenquad.vs.glsl", GL_VERTEX_SHADER));
			CallOnClose([] { s_quadVertexShaderModule.Destroy(); });
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
