#pragma once

#include "functions.h"

#include "glresource.h"

namespace TankGame
{
	void DeleteTexture(GLuint id);
	
	enum class TextureFormat
	{
		Depth16,
		Depth32,
		R8,
		RG8,
		RGBA8,
		R32F,
		RG32F,
		RGBA32F,
		RG16F,
		RGBA16F,
		R8UI
	};
	
	static constexpr size_t NUM_TEXTURE_FORMATS = (size_t)TextureFormat::R8UI + 1;
	
	namespace TextureFormatGL
	{
		extern const GLenum Type[NUM_TEXTURE_FORMATS];
		extern const GLenum Format[NUM_TEXTURE_FORMATS];
		extern const GLenum InternalFormat[NUM_TEXTURE_FORMATS];
		extern const int    NumComponents[NUM_TEXTURE_FORMATS];
		extern const size_t BytesPerPixel[NUM_TEXTURE_FORMATS];
	}
	
	class Texture : public GLResource<DeleteTexture>
	{
	public:
		inline Texture(GLsizei levels, TextureFormat format)
		    : m_levels(levels), m_format(format) { }
		
		inline static int GetMipmapCount(int maxDimension)
		{
			return (int)std::log2(maxDimension) + 1;
		}
		
		void SetupMipmapping(bool generateMipmaps);
		
		inline void Bind(GLuint unit) const
		{ glBindTextureUnit(unit, GetID()); }
		
		inline void SetMinFilter(int minFilter)
		{ glTextureParameteri(GetID(), GL_TEXTURE_MIN_FILTER, minFilter); }
		inline void SetMagFilter(int magFilter)
		{ glTextureParameteri(GetID(), GL_TEXTURE_MAG_FILTER, magFilter); }
		
		inline GLsizei GetLevels() const
		{ return m_levels; }
		
		TextureFormat GetFormat() const
		{ return m_format; }
		
		int GetNumComponents() const
		{ return TextureFormatGL::NumComponents[(int)m_format]; }
		
	private:
		GLsizei m_levels;
		TextureFormat m_format;
	};
}
