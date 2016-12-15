#pragma once

#include "functions.h"
#include <cmath>

#include "glresource.h"

namespace TankGame
{
	void DeleteTexture(GLuint id);
	
	class Texture : public GLResource<DeleteTexture>
	{
	public:
		inline Texture(GLsizei levels, GLenum internalFormat)
		    : m_levels(levels), m_internalFormat(internalFormat) { }
		
		inline static int GetMipmapCount(int maxDimension)
		{
			return static_cast<int>(std::ceil(std::log2(maxDimension)));
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
		GLenum GetInternalFormat() const
		{ return m_internalFormat; }
		
	private:
		GLsizei m_levels;
		GLenum m_internalFormat;
	};
}
