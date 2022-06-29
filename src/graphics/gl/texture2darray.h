#pragma once


#include "texture.h"

namespace TankGame
{
	class Texture2DArray : public Texture
	{
	public:
		Texture2DArray(GLsizei width, GLsizei height, GLsizei layers, GLsizei levels, TextureFormat format);
		
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
		
	private:
		GLsizei m_width;
		GLsizei m_height;
		GLsizei m_layers;
	};
}
