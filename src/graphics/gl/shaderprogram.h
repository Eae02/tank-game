#pragma once

#include "glresource.h"

#include <initializer_list>
#include <string>

namespace TankGame
{
	void DeleteShaderProgram(GLuint id);
	
	class ShaderProgram : public GLResource<DeleteShaderProgram>
	{
	public:
		explicit ShaderProgram(std::initializer_list<const class ShaderModule*> modules);
		
		int GetUniformLocation(const std::string& name) const;
		int GetUniformBlockIndex(const std::string& name) const;
		
		void SetUniformBlockBinding(const std::string& name, GLuint binding, bool failIfMissing = true);
		void SetTextureBinding(const std::string& name, GLuint binding, bool failIfMissing = true);
		
		void Use() const;
		
		inline static GLint GetCurrentShaderProgram()
		{ return s_currentShaderProgram; }
		
	private:
		static GLint s_currentShaderProgram;
	};
}
