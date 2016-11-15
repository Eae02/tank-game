#include "shaderprogram.h"
#include "shadermodule.h"
#include <stdexcept>

namespace TankGame
{
	GLint ShaderProgram::s_currentShaderProgram = 0;
	
	ShaderProgram::ShaderProgram(std::initializer_list<const ShaderModule*> modules)
	    : GLResource(glCreateProgram())
	{
		for (const ShaderModule* shaderModule : modules)
		{
			glAttachShader(GetID(), shaderModule->GetID());
		}
		
		glLinkProgram(GetID());
		
		GLint linkStatus;
		glGetProgramiv(GetID(), GL_LINK_STATUS, &linkStatus);
		
		if (linkStatus == 0)
		{
			char log[1024];
			GLsizei length;
			
			glGetProgramInfoLog(GetID(), sizeof(log), &length, log);
			
			throw std::runtime_error(log);
		}
	}
	
	int ShaderProgram::GetUniformLocation(const std::string& name) const
	{
		return glGetUniformLocation(GetID(), name.c_str());
	}
	
	int ShaderProgram::GetUniformBlockIndex(const std::string& name) const
	{
		return glGetUniformBlockIndex(GetID(), name.c_str());
	}
	
	void ShaderProgram::Use() const
	{
		if (s_currentShaderProgram == GetID())
			return;
		s_currentShaderProgram = GetID();
		glUseProgram(GetID());
	}
	
	void DeleteShaderProgram(GLuint id)
	{
		glDeleteProgram(id);
	}
}
