#include "shadermodule.h"
#include "specializationinfo.h"
#include "../../utils/ioutils.h"

#include <cstddef>
#include <cstring>
#include <sstream>
#include <algorithm>

namespace TankGame
{
	ShaderModule::ShaderModule(const std::string& source, GLenum type, const SpecializationInfo* specializationInfo)
	    : GLResource(glCreateShader(type))
	{
		int numStrings;
		
		GLint lengths[3];
		const GLchar* strings[3];
		
		std::string specString;
		if (specializationInfo != nullptr)
		{
			const char* VERSION_DIRECTIVE = "#version";
			bool hasVersion = source.compare(0, strlen(VERSION_DIRECTIVE), VERSION_DIRECTIVE) == 0;
			
			specString = specializationInfo->GetSourceString();
			
			if (hasVersion)
			{
				size_t firstLineLen = source.find('\n');
				
				lengths[0] = static_cast<GLint>(firstLineLen + 1);
				strings[0] = source.c_str();
				
				lengths[1] = static_cast<GLint>(specString.length());
				strings[1] = specString.c_str();
				
				lengths[2] = static_cast<GLint>(source.length() - firstLineLen);
				strings[2] = source.c_str() + firstLineLen;
				
				numStrings = 3;
			}
			else
			{
				lengths[0] = static_cast<GLint>(specString.length());
				strings[0] = specString.c_str();
				
				lengths[1] = static_cast<GLint>(source.length());
				strings[1] = source.c_str();
				
				numStrings = 2;
			}
		}
		else
		{
			lengths[0] = static_cast<GLint>(source.length());
			strings[0] = source.c_str();
			numStrings = 1;
		}
		
		glShaderSource(GetID(), numStrings, strings, lengths);
		glCompileShader(GetID());
		
		GLint compileStatus;
		glGetShaderiv(GetID(), GL_COMPILE_STATUS, &compileStatus);
		
		if (compileStatus == 0)
		{
			char log[1024];
			GLsizei length;
			
			glGetShaderInfoLog(GetID(), sizeof(log), &length, log);
			
			throw std::runtime_error(log);
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
	
	ShaderModule ShaderModule::FromFile(const fs::path& path, GLenum type,
	                                    const SpecializationInfo* specializationInfo)
	{
		try
		{
			return ShaderModule(ReadAndPreProcessSource(path), type, specializationInfo);
		}
		catch (const std::runtime_error& ex)
		{
			throw std::runtime_error("Shader from " + path.string() + " failed to compile:\n" + ex.what());
		}
	}
	
	void DeleteShaderModule(GLuint id)
	{
		glDeleteShader(id);
	}
}
