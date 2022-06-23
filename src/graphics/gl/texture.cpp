#include "texture.h"
#include "dsawrapper.h"

namespace TankGame
{
	const GLenum TextureFormatGL::Type[NUM_TEXTURE_FORMATS] = 
	{
		/* Depth16 */ GL_UNSIGNED_SHORT,
		/* Depth32 */ GL_FLOAT,
		/* R8      */ GL_UNSIGNED_BYTE,
		/* RG8     */ GL_UNSIGNED_BYTE,
		/* RGBA8   */ GL_UNSIGNED_BYTE,
		/* R32F    */ GL_FLOAT,
		/* RG32F   */ GL_FLOAT,
		/* RGBA32F */ GL_FLOAT,
		/* RG16F   */ GL_FLOAT,
		/* RGBA16F */ GL_FLOAT,
		/* R8UI    */ GL_UNSIGNED_BYTE,
	};
	
	const GLenum TextureFormatGL::Format[NUM_TEXTURE_FORMATS] = 
	{
		/* Depth16 */ GL_DEPTH_COMPONENT,
		/* Depth32 */ GL_DEPTH_COMPONENT,
		/* R8      */ GL_RED,
		/* RG8     */ GL_RG,
		/* RGBA8   */ GL_RGBA,
		/* R32F    */ GL_RED,
		/* RG32F   */ GL_RG,
		/* RGBA32F */ GL_RGBA,
		/* RG16F   */ GL_RG,
		/* RGBA16F */ GL_RGBA,
		/* R8UI    */ GL_RED_INTEGER,
	};
	
	const GLenum TextureFormatGL::InternalFormat[NUM_TEXTURE_FORMATS] = 
	{
		/* Depth16 */ GL_DEPTH_COMPONENT16,
		/* Depth32 */ GL_DEPTH_COMPONENT32F,
		/* R8      */ GL_R8,
		/* RG8     */ GL_RG8,
		/* RGBA8   */ GL_RGBA8,
		/* R32F    */ GL_R32F,
		/* RG32F   */ GL_RG32F,
		/* RGBA32F */ GL_RGBA32F,
		/* RG16F   */ GL_RG16F,
		/* RGBA16F */ GL_RGBA16F,
		/* R8UI    */ GL_R8UI,
	};
	
	const int TextureFormatGL::NumComponents[NUM_TEXTURE_FORMATS] = 
	{
		/* Depth16 */ 1,
		/* Depth32 */ 1,
		/* R8      */ 1,
		/* RG8     */ 2,
		/* RGBA8   */ 4,
		/* R32F    */ 1,
		/* RG32F   */ 2,
		/* RGBA32F */ 4,
		/* RG16F   */ 2,
		/* RGBA16F */ 4,
		/* R8UI    */ 1,
	};
	
	const size_t TextureFormatGL::BytesPerPixel[NUM_TEXTURE_FORMATS] = 
	{
		/* Depth16 */ 2,
		/* Depth32 */ 4,
		/* R8      */ 1,
		/* RG8     */ 2,
		/* RGBA8   */ 4,
		/* R32F    */ 4,
		/* RG32F   */ 8,
		/* RGBA32F */ 16,
		/* RG16F   */ 2,
		/* RGBA16F */ 8,
		/* R8UI    */ 1,
	};
	
	void Texture::SetupMipmapping(bool generateMipmaps)
	{
		if (generateMipmaps)
		{
			glGenerateTextureMipmap(GetID());
			SetMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		}
		else
		{
			glTextureParameteri(GetID(), GL_TEXTURE_MAX_LEVEL, 0);
		}
	}
	
	void DeleteTexture(GLuint id)
	{
		if (DSAWrapper::IsActive())
			DSAWrapper::DeleteTexture(id);
		glDeleteTextures(1, &id);
	}
}
