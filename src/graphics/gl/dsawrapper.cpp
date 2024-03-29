#include "dsawrapper.h"
#include "shaderprogram.h"
#include "framebuffer.h"

#include <unordered_map>

namespace TankGame
{
	namespace DSAWrapper
	{
		static bool active = false;
		bool IsActive()
		{ return active; }
		
		void InstallDSAFunctions()
		{
			active = true;
			
			glBindTextureUnit = BindTextureUnit;
			glTextureParameteri = TextureParameteri;
			glTextureParameterf = TextureParameterf;
			glGenerateTextureMipmap = GenerateTextureMipmap;
			glCreateTextures = CreateTextures;
			glTextureStorage2D = TextureStorage2D;
			glTextureSubImage2D = TextureSubImage2D;
			glTextureStorage3D = TextureStorage3D;
			glTextureSubImage3D = TextureSubImage3D;
			glGetTextureImage = GetTextureImage;
			
			glCreateFramebuffers = CreateFramebuffers;
			glClearNamedFramebufferfv = ClearNamedFramebufferfv;
			glNamedFramebufferTexture = NamedFramebufferTexture;
			glNamedFramebufferDrawBuffer = NamedFramebufferDrawBuffer;
			glNamedFramebufferDrawBuffers = NamedFramebufferDrawBuffers;
			
			glProgramUniform1f = ProgramUniform1f;
			glProgramUniform1i = ProgramUniform1i;
			glProgramUniform2f = ProgramUniform2f;
			glProgramUniform2i = ProgramUniform2i;
			glProgramUniform3f = ProgramUniform3f;
			glProgramUniform3i = ProgramUniform3i;
			glProgramUniform4f = ProgramUniform4f;
			glProgramUniform4i = ProgramUniform4i;
			glProgramUniform1fv = ProgramUniform1fv;
			glProgramUniform1iv = ProgramUniform1iv;
			glProgramUniform2fv = ProgramUniform2fv;
			glProgramUniform2iv = ProgramUniform2iv;
			glProgramUniform3fv = ProgramUniform3fv;
			glProgramUniform3iv = ProgramUniform3iv;
			glProgramUniform4fv = ProgramUniform4fv;
			glProgramUniform4iv = ProgramUniform4iv;
			glProgramUniformMatrix3fv = ProgramUniformMatrix3fv;
			glProgramUniformMatrix4fv = ProgramUniformMatrix4fv;
			
			glCreateBuffers = glGenBuffers;
			glFlushMappedNamedBufferRange = nullptr;
			glNamedBufferSubData = nullptr;
		}
		
		static std::unordered_map<GLuint, GLenum> textureTargets;
		
		void DeleteTexture(GLuint texture)
		{
			textureTargets.erase(texture);
		}
		
		static GLenum BindTexture(GLuint texture)
		{
			GLenum target = textureTargets.at(texture);
			glBindTexture(target, texture);
			return target;
		}
		
		void APIENTRY BindTextureUnit(GLuint unit, GLuint texture)
		{
			glActiveTexture(GL_TEXTURE0 + unit);
			BindTexture(texture);
		}
		
		void APIENTRY TextureParameteri(GLuint texture, GLenum pname, GLint param)
		{
			GLenum target = BindTexture(texture);
			glTexParameteri(target, pname, param);
		}
		
		void APIENTRY TextureParameterf(GLuint texture, GLenum pname, GLfloat param)
		{
			GLenum target = BindTexture(texture);
			glTexParameterf(target, pname, param);
		}
		
		void APIENTRY GenerateTextureMipmap(GLuint texture)
		{
			GLenum target = BindTexture(texture);
			glGenerateMipmap(target);
		}
		
		void APIENTRY CreateTextures(GLenum target, GLsizei n, GLuint* textures)
		{
			glGenTextures(n, textures);
			for (GLsizei i = 0; i < n; i++)
			{
				textureTargets.emplace(textures[i], target);
			}
		}
		
		void APIENTRY TextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
		{
			GLenum target = BindTexture(texture);
			glTexStorage2D(target, levels, internalformat, width, height);
		}
		
		void APIENTRY TextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
		                       GLenum format, GLenum type, const void* pixels)
		{
			GLenum target = BindTexture(texture);
			glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
		}
		
		void APIENTRY TextureStorage3D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height,
		                      GLsizei depth)
		{
			GLenum target = BindTexture(texture);
			glTexStorage3D(target, levels, internalformat, width, height, depth);
		}
		
		void APIENTRY TextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
		                       GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
		{
			GLenum target = BindTexture(texture);
			glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
		}
		
		void APIENTRY GetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels)
		{
			GLenum target = BindTexture(texture);
			glGetnTexImage(target, level, format, type, bufSize, pixels);
		}
		
		void APIENTRY CreateFramebuffers(GLsizei n, GLuint* framebuffers)
		{
			glGenFramebuffers(n, framebuffers);
		}
		
		template <typename CallbackTp>
		inline void BindFramebufferAndInvoke(GLuint framebuffer, CallbackTp callback)
		{
			bool different = Framebuffer::GetCurrentFramebufferID() != framebuffer;
			if (different)
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
			callback();
			if (different)
				glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer::GetCurrentFramebufferID());
		}
		
		void APIENTRY ClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value)
		{
			BindFramebufferAndInvoke(framebuffer, [=] { glClearBufferfv(buffer, drawbuffer, value); });
		}
		
		void APIENTRY NamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level)
		{
			BindFramebufferAndInvoke(framebuffer, [=]
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, level);
			});
		}
		
		void APIENTRY NamedFramebufferDrawBuffer(GLuint framebuffer, GLenum buf)
		{
			BindFramebufferAndInvoke(framebuffer, [=]
			{
				if (glDrawBuffer)
					glDrawBuffer(buf);
				else
					glDrawBuffers(1, &buf);
			});
		}
		
		void APIENTRY NamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum* bufs)
		{
			BindFramebufferAndInvoke(framebuffer, [=] { glDrawBuffers(n, bufs); });
		}
		
		
		template <typename CallbackTp>
		inline void UseProgramAndInvoke(GLuint program, CallbackTp callback)
		{
			bool different = ShaderProgram::GetCurrentShaderProgram() != static_cast<GLint>(program);
			if (different)
				glUseProgram(program);
			callback();
			if (different)
				glUseProgram(ShaderProgram::GetCurrentShaderProgram());
		}
		
		void APIENTRY ProgramUniform1f(GLuint program, GLint location, GLfloat v0)
		{
			UseProgramAndInvoke(program, [=] { glUniform1f(location, v0); });
		}
		
		void APIENTRY ProgramUniform1i(GLuint program, GLint location, GLint v0)
		{
			UseProgramAndInvoke(program, [=] { glUniform1i(location, v0); });
		}
		
		void APIENTRY ProgramUniform2f(GLuint program, GLint location, GLfloat v0, GLfloat v1)
		{
			UseProgramAndInvoke(program, [=] { glUniform2f(location, v0, v1); });
		}
		
		void APIENTRY ProgramUniform2i(GLuint program, GLint location, GLint v0, GLint v1)
		{
			UseProgramAndInvoke(program, [=] { glUniform2i(location, v0, v1); });
		}
		
		void APIENTRY ProgramUniform3f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
		{
			UseProgramAndInvoke(program, [=] { glUniform3f(location, v0, v1, v2); });
		}
		
		void APIENTRY ProgramUniform3i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2)
		{
			UseProgramAndInvoke(program, [=] { glUniform3i(location, v0, v1, v2); });
		}
		
		void APIENTRY ProgramUniform4f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
		{
			UseProgramAndInvoke(program, [=] { glUniform4f(location, v0, v1, v2, v3); });
		}
		
		void APIENTRY ProgramUniform4i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
		{
			UseProgramAndInvoke(program, [=] { glUniform4i(location, v0, v1, v2, v3); });
		}
		
		void APIENTRY ProgramUniform1fv(GLuint program, GLint location, GLsizei count, const GLfloat* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform1fv(location, count, value); });
		}
		
		void APIENTRY ProgramUniform1iv(GLuint program, GLint location, GLsizei count, const GLint* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform1iv(location, count, value); });
		}
		
		void APIENTRY ProgramUniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform2fv(location, count, value); });
		}
		
		void APIENTRY ProgramUniform2iv(GLuint program, GLint location, GLsizei count, const GLint* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform2iv(location, count, value); });
		}
		
		void APIENTRY ProgramUniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform3fv(location, count, value); });
		}
		
		void APIENTRY ProgramUniform3iv(GLuint program, GLint location, GLsizei count, const GLint* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform3iv(location, count, value); });
		}
		
		void APIENTRY ProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform4fv(location, count, value); });
		}
		
		void APIENTRY ProgramUniform4iv(GLuint program, GLint location, GLsizei count, const GLint* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform4iv(location, count, value); });
		}
		
		void APIENTRY ProgramUniformMatrix3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
		{
			UseProgramAndInvoke(program, [=] { glUniformMatrix3fv(location, count, transpose, value); });
		}
		
		void APIENTRY ProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
		{
			UseProgramAndInvoke(program, [=] { glUniformMatrix4fv(location, count, transpose, value); });
		}
	}
}
