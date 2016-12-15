#pragma once

#include "functions.h"

namespace TankGame
{
	namespace DSAWrapper
	{
		bool IsActive();
		void DeleteTexture(GLuint texture);
		
		void InstallDSAFunctions();
		
		void BindTextureUnit(GLuint unit, GLuint texture);
		void TextureParameteri(GLuint texture, GLenum pname, GLint param);
		void TextureParameterf(GLuint texture, GLenum pname, GLfloat param);
		void GenerateTextureMipmap(GLuint texture);
		void CreateTextures(GLenum target, GLsizei n, GLuint* textures);
		void TextureStorage1D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width);
		void TextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void TextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
		void TextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void TextureStorage3D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
		void TextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
		void GetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels);
		
		void CreateVertexArrays(GLsizei n, GLuint* arrays);
		void EnableVertexArrayAttrib(GLuint vaobj, GLuint index);
		void VertexArrayAttribBinding(GLuint vaobj, GLuint attribindex, GLuint bindingindex);
		void VertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
		void VertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
		void VertexArrayElementBuffer(GLuint vaobj, GLuint buffer);
		void VertexArrayBindingDivisor(GLuint vaobj, GLuint bindingindex, GLuint divisor);
		
		void CreateFramebuffers(GLsizei n, GLuint* framebuffers);
		void ClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value);
		void NamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
		void NamedFramebufferDrawBuffer(GLuint framebuffer, GLenum buf);
		void NamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum* bufs);
		void NamedFramebufferRenderbuffer(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
		
		void CreateRenderbuffers(GLsizei n, GLuint* renderbuffers);
		void NamedRenderbufferStorage(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
		
		void ProgramUniform1f(GLuint program, GLint location, GLfloat v0);
		void ProgramUniform1i(GLuint program, GLint location, GLint v0);
		void ProgramUniform2f(GLuint program, GLint location, GLfloat v0, GLfloat v1);
		void ProgramUniform2i(GLuint program, GLint location, GLint v0, GLint v1);
		void ProgramUniform3f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
		void ProgramUniform3i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
		void ProgramUniform4f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
		void ProgramUniform4i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
		void ProgramUniform1fv(GLuint program, GLint location, GLsizei count, const GLfloat* value);
		void ProgramUniform1iv(GLuint program, GLint location, GLsizei count, const GLint* value);
		void ProgramUniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat* value);
		void ProgramUniform2iv(GLuint program, GLint location, GLsizei count, const GLint* value);
		void ProgramUniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat* value);
		void ProgramUniform3iv(GLuint program, GLint location, GLsizei count, const GLint* value);
		void ProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat* value);
		void ProgramUniform4iv(GLuint program, GLint location, GLsizei count, const GLint* value);
		void ProgramUniformMatrix3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		void ProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		
		void CreateBuffers(GLsizei n, GLuint* buffers);
		void NamedBufferStorage(GLuint buffer, GLsizeiptr size, const void* data, GLbitfield flags);
		void* MapNamedBuffer(GLuint buffer, GLenum access);
		void* MapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
		GLboolean UnmapNamedBuffer(GLuint buffer);
	}
}
