#pragma once

#include "functions.h"

namespace TankGame
{
	namespace DSAWrapper
	{
		bool IsActive();
		void DeleteTexture(GLuint texture);
		
		void InstallDSAFunctions();
		
		void APIENTRY BindTextureUnit(GLuint unit, GLuint texture);
		void APIENTRY TextureParameteri(GLuint texture, GLenum pname, GLint param);
		void APIENTRY TextureParameterf(GLuint texture, GLenum pname, GLfloat param);
		void APIENTRY GenerateTextureMipmap(GLuint texture);
		void APIENTRY CreateTextures(GLenum target, GLsizei n, GLuint* textures);
		void APIENTRY TextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
		void APIENTRY TextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void APIENTRY TextureStorage3D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
		void APIENTRY TextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
		void APIENTRY GetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels);
		
		void APIENTRY CreateFramebuffers(GLsizei n, GLuint* framebuffers);
		void APIENTRY ClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value);
		void APIENTRY NamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
		void APIENTRY NamedFramebufferDrawBuffer(GLuint framebuffer, GLenum buf);
		void APIENTRY NamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum* bufs);
		
		void APIENTRY ProgramUniform1f(GLuint program, GLint location, GLfloat v0);
		void APIENTRY ProgramUniform1i(GLuint program, GLint location, GLint v0);
		void APIENTRY ProgramUniform2f(GLuint program, GLint location, GLfloat v0, GLfloat v1);
		void APIENTRY ProgramUniform2i(GLuint program, GLint location, GLint v0, GLint v1);
		void APIENTRY ProgramUniform3f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
		void APIENTRY ProgramUniform3i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
		void APIENTRY ProgramUniform4f(GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
		void APIENTRY ProgramUniform4i(GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
		void APIENTRY ProgramUniform1fv(GLuint program, GLint location, GLsizei count, const GLfloat* value);
		void APIENTRY ProgramUniform1iv(GLuint program, GLint location, GLsizei count, const GLint* value);
		void APIENTRY ProgramUniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat* value);
		void APIENTRY ProgramUniform2iv(GLuint program, GLint location, GLsizei count, const GLint* value);
		void APIENTRY ProgramUniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat* value);
		void APIENTRY ProgramUniform3iv(GLuint program, GLint location, GLsizei count, const GLint* value);
		void APIENTRY ProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat* value);
		void APIENTRY ProgramUniform4iv(GLuint program, GLint location, GLsizei count, const GLint* value);
		void APIENTRY ProgramUniformMatrix3fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		void APIENTRY ProgramUniformMatrix4fv(GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
	}
}
