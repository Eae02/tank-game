#include "shadermodule.h"
#include "specializationinfo.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"

#include <sstream>

namespace TankGame
{
	ShaderModule::ShaderModule(const std::string& source, GLenum type, const SpecializationInfo* specializationInfo)
	    : GLResource(glCreateShader(type))
	{
		std::string fullSource;
#ifdef __EMSCRIPTEN__
		fullSource +=
			"#version 300 es\n"
			"precision lowp sampler2DArray;\n"
			"precision lowp usampler2D;\n"
			"precision highp float;\n"
			"precision highp int;\n";
#else
		fullSource += "#version 330 core\n";
#endif
		
		if (specializationInfo != nullptr)
			fullSource += specializationInfo->GetSourceString();
		fullSource += "\n#line 1\n";
		fullSource += source;
		
		const GLchar* stringPtr = fullSource.c_str();
		glShaderSource(GetID(), 1, &stringPtr, nullptr);
		glCompileShader(GetID());
		
		GLint compileStatus;
		glGetShaderiv(GetID(), GL_COMPILE_STATUS, &compileStatus);
		
		if (compileStatus == 0)
		{
			char log[1024];
			GLsizei length;
			
			glGetShaderInfoLog(GetID(), sizeof(log), &length, log);
			
			Panic(log);
		}
	}
	
	std::string ShaderModule::ReadAndPreProcessSource(const fs::path& path)
	{
		fs::path parentPath = path.parent_path();
		std::string source = ReadFileContents(path);
		std::ostringstream result;
		
		size_t lineNumber = 0;
		size_t lineEnd;
		
		const char* INCLUDE_DIRECTIVE = "#include ";
		
		for (size_t i = 0; i < source.length(); i = lineEnd + 1)
		{
			lineNumber++;
			
			lineEnd = source.find('\n', i);
			if (lineEnd == std::string::npos)
				lineEnd = source.length();
			
			const char* line = source.data() + i;
			if (lineEnd != i && memcmp(line, INCLUDE_DIRECTIVE, std::min(strlen(INCLUDE_DIRECTIVE), lineEnd - i)) == 0)
			{
				size_t includePathBegin = i + strlen(INCLUDE_DIRECTIVE);
				std::string includeTarget = source.substr(includePathBegin, lineEnd - includePathBegin);
				if (!includeTarget.empty() && includeTarget.front() == '"')
					includeTarget.erase(includeTarget.begin());
				if (!includeTarget.empty() && includeTarget.back() == '"')
					includeTarget.pop_back();
				fs::path targetPath = parentPath / includeTarget;
				
				result << ReadAndPreProcessSource(targetPath) << "\n#line " << lineNumber << "\n";
			}
			else
			{
				result.write(line, lineEnd - i + 1);
			}
		}
		
		return result.str();
	}
	
	ShaderModule ShaderModule::FromFile(const fs::path& path, GLenum type, const SpecializationInfo* specializationInfo)
	{
		return ShaderModule(ReadAndPreProcessSource(path), type, specializationInfo);
	}
	
	ShaderModule ShaderModule::FromResFile(const std::string& name, const class SpecializationInfo* specializationInfo)
	{
		GLenum type;
		if (name.ends_with(".fs.glsl"))
			type = GL_FRAGMENT_SHADER;
		else if (name.ends_with(".vs.glsl"))
			type = GL_VERTEX_SHADER;
		else
			Panic("Unknown shader type: '" + name + "'.");
		
		return ShaderModule::FromFile(resDirectoryPath / "shaders" / name, type, specializationInfo);
	}
	
	void DeleteShaderModule(GLuint id)
	{
		glDeleteShader(id);
	}
}
