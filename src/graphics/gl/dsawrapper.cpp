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
			glTextureStorage1D = TextureStorage1D;
			glTextureSubImage1D = TextureSubImage1D;
			glTextureStorage2D = TextureStorage2D;
			glTextureSubImage2D = TextureSubImage2D;
			glTextureStorage3D = TextureStorage3D;
			glTextureSubImage3D = TextureSubImage3D;
			glGetTextureImage = GetTextureImage;
			
			glCreateVertexArrays = CreateVertexArrays;
			glEnableVertexArrayAttrib = EnableVertexArrayAttrib;
			glVertexArrayAttribBinding = VertexArrayAttribBinding;
			glVertexArrayAttribFormat = VertexArrayAttribFormat;
			glVertexArrayVertexBuffer = VertexArrayVertexBuffer;
			glVertexArrayElementBuffer = VertexArrayElementBuffer;
			glVertexArrayBindingDivisor = VertexArrayBindingDivisor;
			
			glCreateFramebuffers = CreateFramebuffers;
			glClearNamedFramebufferfv = ClearNamedFramebufferfv;
			glNamedFramebufferTexture = NamedFramebufferTexture;
			glNamedFramebufferDrawBuffer = NamedFramebufferDrawBuffer;
			glNamedFramebufferDrawBuffers = NamedFramebufferDrawBuffers;
			glNamedFramebufferRenderbuffer = NamedFramebufferRenderbuffer;
			
			glCreateRenderbuffers = CreateRenderbuffers;
			glNamedRenderbufferStorage = NamedRenderbufferStorage;
			
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
			
			glCreateBuffers = CreateBuffers;
			glNamedBufferStorage = NamedBufferStorage;
			glMapNamedBuffer = MapNamedBuffer;
			glMapNamedBufferRange = MapNamedBufferRange;
			glUnmapNamedBuffer = UnmapNamedBuffer;
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
		
		void BindTextureUnit(GLuint unit, GLuint texture)
		{
			glActiveTexture(GL_TEXTURE0 + unit);
			BindTexture(texture);
		}
		
		void TextureParameteri(GLuint texture, GLenum pname, GLint param)
		{
			GLenum target = BindTexture(texture);
			glTexParameteri(target, pname, param);
		}
		
		void TextureParameterf(GLuint texture, GLenum pname, GLfloat param)
		{
			GLenum target = BindTexture(texture);
			glTexParameterf(target, pname, param);
		}
		
		void GenerateTextureMipmap(GLuint texture)
		{
			GLenum target = BindTexture(texture);
			glGenerateMipmap(target);
		}
		
		void CreateTextures(GLenum target, GLsizei n, GLuint* textures)
		{
			glGenTextures(n, textures);
			for (GLsizei i = 0; i < n; i++)
			{
				textureTargets.emplace(textures[i], target);
			}
		}
		
		void TextureStorage1D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width)
		{
			GLenum target = BindTexture(texture);
			glTexStorage1D(target, levels, internalformat, width);
		}
		
		void TextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type,
		                       const void* pixels)
		{
			GLenum target = BindTexture(texture);
			glTexSubImage1D(target, level, xoffset, width, format, type, pixels);
		}
		
		void TextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
		{
			GLenum target = BindTexture(texture);
			glTexStorage2D(target, levels, internalformat, width, height);
		}
		
		void TextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
		                       GLenum format, GLenum type, const void* pixels)
		{
			GLenum target = BindTexture(texture);
			glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
		}
		
		void TextureStorage3D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height,
		                      GLsizei depth)
		{
			GLenum target = BindTexture(texture);
			glTexStorage3D(target, levels, internalformat, width, height, depth);
		}
		
		void TextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width,
		                       GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
		{
			GLenum target = BindTexture(texture);
			glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
		}
		
		void GetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels)
		{
			GLenum target = BindTexture(texture);
			glGetnTexImage(target, level, format, type, bufSize, pixels);
		}
		
		void CreateVertexArrays(GLsizei n, GLuint* arrays)
		{
			glGenVertexArrays(n, arrays);
		}
		
		void EnableVertexArrayAttrib(GLuint vaobj, GLuint index)
		{
			glBindVertexArray(vaobj);
			glEnableVertexAttribArray(index);
		}
		
		void VertexArrayAttribBinding(GLuint vaobj, GLuint attribindex, GLuint bindingindex)
		{
			glBindVertexArray(vaobj);
			glVertexAttribBinding(attribindex, bindingindex);
		}
		
		void VertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized,
		                             GLuint relativeoffset)
		{
			glBindVertexArray(vaobj);
			glVertexAttribFormat(attribindex, size, type, normalized, relativeoffset);
		}
		
		void VertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride)
		{
			glBindVertexArray(vaobj);
			glBindVertexBuffer(bindingindex, buffer, offset, stride);
		}
		
		void VertexArrayElementBuffer(GLuint vaobj, GLuint buffer)
		{
			glBindVertexArray(vaobj);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
		}
		
		void VertexArrayBindingDivisor(GLuint vaobj, GLuint bindingindex, GLuint divisor)
		{
			glBindVertexArray(vaobj);
			glVertexBindingDivisor(bindingindex, divisor);
		}
		
		void CreateFramebuffers(GLsizei n, GLuint* framebuffers)
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
		
		void ClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value)
		{
			BindFramebufferAndInvoke(framebuffer, [=] { glClearBufferfv(buffer, drawbuffer, value); });
		}
		
		void NamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level)
		{
			BindFramebufferAndInvoke(framebuffer, [=]
			{
				glFramebufferTexture(GL_FRAMEBUFFER, attachment, texture, level);
			});
		}
		
		void NamedFramebufferDrawBuffer(GLuint framebuffer, GLenum buf)
		{
			BindFramebufferAndInvoke(framebuffer, [=] { glDrawBuffer(buf); });
		}
		
		void NamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum* bufs)
		{
			BindFramebufferAndInvoke(framebuffer, [=] { glDrawBuffers(n, bufs); });
		}
		
		void NamedFramebufferRenderbuffer(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget,
		                                  GLuint renderbuffer)
		{
			BindFramebufferAndInvoke(framebuffer, [=]
			{
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, renderbuffertarget, renderbuffer);
			});
		}
		
		
		void CreateRenderbuffers(GLsizei n, GLuint* renderbuffers)
		{
			glGenRenderbuffers(n, renderbuffers);
		}
		
		void NamedRenderbufferStorage(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
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
		
		void ProgramUniform1f(GLuint program, GLint location, GLfloat v0)
		{
			UseProgramAndInvoke(program, [=] { glUniform1f(location, v0); });
		}
		
		void ProgramUniform1i(GLuint program, GLint location, GLint v0)
		{
			UseProgramAndInvoke(program, [=] { glUniform1i(location, v0); });
		}
		
		void ProgramUniform2f(GLuint program, GLint location, GLfloat v0, GLfloat v1)
		{
			UseProgramAndInvoke(program, [=] { glUniform2f(location, v0, v1); });
		}
		
		void ProgramUniform2i(GLuint program, GLint location, GLint v0, GLint v1)
		{
			UseProgramAndInvoke(program, [=] { glUniform2i(location, v0, v1); });
		}
		
		void ProgramUniform3f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2)
		{
			UseProgramAndInvoke(program, [=] { glUniform3f(location, v0, v1, v2); });
		}
		
		void ProgramUniform3i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2)
		{
			UseProgramAndInvoke(program, [=] { glUniform3i(location, v0, v1, v2); });
		}
		
		void ProgramUniform4f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
		{
			UseProgramAndInvoke(program, [=] { glUniform4f(location, v0, v1, v2, v3); });
		}
		
		void ProgramUniform4i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3)
		{
			UseProgramAndInvoke(program, [=] { glUniform4i(location, v0, v1, v2, v3); });
		}
		
		void ProgramUniform1fv(GLuint program, GLint location, GLsizei count, const GLfloat* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform1fv(location, count, value); });
		}
		
		void ProgramUniform1iv(GLuint program, GLint location, GLsizei count, const GLint* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform1iv(location, count, value); });
		}
		
		void ProgramUniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform2fv(location, count, value); });
		}
		
		void ProgramUniform2iv(GLuint program, GLint location, GLsizei count, const GLint* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform2iv(location, count, value); });
		}
		
		void ProgramUniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform3fv(location, count, value); });
		}
		
		void ProgramUniform3iv(GLuint program, GLint location, GLsizei count, const GLint* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform3iv(location, count, value); });
		}
		
		void ProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform4fv(location, count, value); });
		}
		
		void ProgramUniform4iv(GLuint program, GLint location, GLsizei count, const GLint* value)
		{
			UseProgramAndInvoke(program, [=] { glUniform4iv(location, count, value); });
		}
		
		void ProgramUniformMatrix3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
		{
			UseProgramAndInvoke(program, [=] { glUniformMatrix3fv(location, count, transpose, value); });
		}
		
		void ProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
		{
			UseProgramAndInvoke(program, [=] { glUniformMatrix4fv(location, count, transpose, value); });
		}
		
		void CreateBuffers(GLsizei n, GLuint* buffers)
		{
			glGenBuffers(n, buffers);
		}
		
		void NamedBufferStorage(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferStorage(GL_ARRAY_BUFFER, size, data, flags);
		}
		
		void* MapNamedBuffer(GLuint buffer, GLenum access)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			return glMapBuffer(GL_ARRAY_BUFFER, access);
		}
		
		void* MapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			return glMapBufferRange(GL_ARRAY_BUFFER, offset, length, access);
		}
		
		GLboolean UnmapNamedBuffer(GLuint buffer)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			return glUnmapBuffer(GL_ARRAY_BUFFER);
		}
	}
}
