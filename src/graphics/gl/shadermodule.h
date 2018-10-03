#pragma once

#include <string>

#include "glresource.h"
#include "../../utils/filesystem.h"

namespace TankGame
{
	void DeleteShaderModule(GLuint id);
	
	class ShaderModule : public GLResource<DeleteShaderModule>
	{
	public:
		ShaderModule(const std::string& source, GLenum type,
		             const class SpecializationInfo* specializationInfo = nullptr);
		
		static ShaderModule FromFile(const fs::path& path, GLenum type,
		                             const class SpecializationInfo* specializationInfo = nullptr);
		
		static std::string ReadAndPreProcessSource(const fs::path& path);
	};
}
