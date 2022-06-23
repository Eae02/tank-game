#include "shaderprogram.h"
#include "shadermodule.h"
#include "../../utils/utils.h"

namespace TankGame
{
	static const std::pair<std::string, GLuint> STANDARD_UNIFORM_BLOCK_BINDINGS[] = 
	{
		{ "RenderSettingsUB", 0 },
		{ "ShieldSettingsUB", 1 },
		{ "ShadowRenderSettingsUB", 2 },
		{ "LightUB", 1 },
	};
	
	GLint ShaderProgram::s_currentShaderProgram = 0;
	
	ShaderProgram::ShaderProgram(const ShaderModule& vertexShader, const ShaderModule* fragmentShader)
		: GLResource(glCreateProgram())
	{
		glAttachShader(GetID(), vertexShader.GetID());
		if (fragmentShader != nullptr)
			glAttachShader(GetID(), fragmentShader->GetID());
		
		glLinkProgram(GetID());
		
		GLint linkStatus;
		glGetProgramiv(GetID(), GL_LINK_STATUS, &linkStatus);
		
		if (linkStatus == 0)
		{
			char log[1024];
			GLsizei length;
			
			glGetProgramInfoLog(GetID(), sizeof(log), &length, log);
			
			Panic(log);
		}
		
		for (auto& [blockName, blockBinding] : STANDARD_UNIFORM_BLOCK_BINDINGS)
		{
			SetUniformBlockBinding(blockName, blockBinding, false);
		}
	}
	
	int ShaderProgram::GetUniformLocation(const std::string& name) const
	{
		int location = glGetUniformLocation(GetID(), name.c_str());
		if (location == -1)
			Panic("Uniform not found: '" + name + "'.");
		return location;
	}
	
	int ShaderProgram::GetUniformBlockIndex(const std::string& name) const
	{
		return glGetUniformBlockIndex(GetID(), name.c_str());
	}
	
	void ShaderProgram::SetUniformBlockBinding(const std::string& name, GLuint binding, bool failIfMissing)
	{
		int index = GetUniformBlockIndex(name);
		if (index != -1)
			glUniformBlockBinding(GetID(), index, binding);
#ifndef NDEBUG
		if (index == -1 && failIfMissing)
			Panic("Uniform block not found: '" + name + "'.");
#endif
	}
	
	void ShaderProgram::SetTextureBinding(const std::string& name, GLuint binding, bool failIfMissing)
	{
		int index = glGetUniformLocation(GetID(), name.c_str());
		if (index != -1)
			glProgramUniform1i(GetID(), index, binding);
#ifndef NDEBUG
		if (index == -1 && failIfMissing)
			Panic("Texture not found: '" + name + "'.");
#endif
	}
	
	void ShaderProgram::Use() const
	{
		if (s_currentShaderProgram == static_cast<GLint>(GetID()))
			return;
		s_currentShaderProgram = GetID();
		glUseProgram(GetID());
	}
	
	void DeleteShaderProgram(GLuint id)
	{
		glDeleteProgram(id);
	}
}
