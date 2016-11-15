#include "tanktextures.h"
#include "utils/ioutils.h"

namespace TankGame
{
	std::unique_ptr<TankTextures> TankTextures::s_instance;
	
	TankTextures::TankTextures()
	    : m_baseDiffuse{ Texture2D::FromFile(GetResDirectory() / "tank" / "base0.png"),
	                     Texture2D::FromFile(GetResDirectory() / "tank" / "base1.png"),
	                     Texture2D::FromFile(GetResDirectory() / "tank" / "base2.png"),
	                     Texture2D::FromFile(GetResDirectory() / "tank" / "base3.png"),
	                     Texture2D::FromFile(GetResDirectory() / "tank" / "base4.png"),
	                     Texture2D::FromFile(GetResDirectory() / "tank" / "base5.png"),
	                     Texture2D::FromFile(GetResDirectory() / "tank" / "base6.png"),
	                     Texture2D::FromFile(GetResDirectory() / "tank" / "base7.png"),
	                     Texture2D::FromFile(GetResDirectory() / "tank" / "base8.png") },
	      m_baseNormals(Texture2D::FromFile(GetResDirectory() / "tank" / "base-normals.png")),
	      m_baseMaterials{ SpriteMaterial(m_baseDiffuse[0], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[1], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[2], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[3], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[4], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[5], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[6], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[7], m_baseNormals, 1, 30),
	                       SpriteMaterial(m_baseDiffuse[8], m_baseNormals, 1, 30)
	      }
	{
		for (Texture2D& texture : m_baseDiffuse)
			texture.SetWrapMode(GL_CLAMP_TO_EDGE);
		m_baseNormals.SetWrapMode(GL_CLAMP_TO_EDGE);
	}
}
