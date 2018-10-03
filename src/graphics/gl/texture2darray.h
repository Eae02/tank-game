#pragma once

#include <string>

#include "texture.h"
#include "../../utils/filesystem.h"

namespace TankGame
{
	class Texture2DArray : public Texture
	{
	public:
		Texture2DArray(GLsizei width, GLsizei height, GLsizei layers, GLsizei levels, GLenum internalFormat);
		
		inline void SetWrapS(int wrapS)
		{ glTextureParameteri(GetID(), GL_TEXTURE_WRAP_S, wrapS); }
		inline void SetWrapT(int wrapT)
		{ glTextureParameteri(GetID(), GL_TEXTURE_WRAP_T, wrapT); }
		inline void SetWrapR(int wrapR)
		{ glTextureParameteri(GetID(), GL_TEXTURE_WRAP_R, wrapR); }
		
		inline GLsizei GetWidth() const
		{ return m_width; }
		inline GLsizei GetHeight() const
		{ return m_height; }
		inline GLsizei GetLayers() const
		{ return m_layers; }
		
		void LoadLayerFromFile(int layer, const fs::path& path);
		
		void ClearLayer(int layer, GLenum format, GLenum type, const void* data);
		
	private:
		GLsizei m_width;
		GLsizei m_height;
		GLsizei m_layers;
	};
}
