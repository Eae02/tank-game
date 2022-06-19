#include "spritematerial.h"
#include "gl/texture2d.h"

namespace TankGame
{
	SpriteMaterial::SpriteMaterial(const Texture2D& diffuse, const Texture2D& normalMap,
	                               float specIntensity, float specExponent)
	    : SpriteMaterial(diffuse, normalMap, specIntensity, specExponent, glm::vec4(1.0f)) { }
	
	SpriteMaterial::SpriteMaterial(const Texture2D& diffuse, const Texture2D& normalMap,
	                               float specIntensity, float specExponent, const glm::vec4& shade)
	    : m_shade(shade), m_specularIntensity(specIntensity), m_specularExponent(specExponent),
	      m_diffuse(diffuse), m_normalMap(normalMap) { }
	
	void SpriteMaterial::Bind(const UniformLocations& uniformLocations) const
	{
		glUniform4fv(uniformLocations.shadeLoc, 1, &m_shade.x);
		glUniform1f(uniformLocations.specularIntensityLoc, m_specularIntensity);
		glUniform1f(uniformLocations.specularExponentLoc, m_specularExponent);
		
		glBindTextureUnit(DIFFUSE_TEXTURE_UNIT, m_diffuse.GetID());
		glBindTextureUnit(NORMAL_MAP_TEXTURE_UNIT, m_normalMap.GetID());
	}
	
	void SpriteMaterial::SetSpecularIntensity(float specularIntensity)
	{
		m_specularIntensity = specularIntensity;
	}
	
	void SpriteMaterial::SetSpecularExponent(float specularExponent)
	{
		m_specularExponent = specularExponent;
	}
	
	void SpriteMaterial::SetShade(const glm::vec4& shade)
	{
		m_shade = shade;
	}
}
