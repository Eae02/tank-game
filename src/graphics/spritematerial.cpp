#include "spritematerial.h"
#include "gl/texture2d.h"

namespace TankGame
{
	SpriteMaterial::SpriteMaterial(const Texture2D& diffuse, const Texture2D& normalMap,
	                               float specIntensity, float specExponent)
	    : SpriteMaterial(diffuse, normalMap, specIntensity, specExponent, glm::vec4(1.0f)) { }
	
	static const GLenum uniformBufferFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
	
	static const GLuint uniformBufferSize = sizeof(float) * 6;
	
	SpriteMaterial::SpriteMaterial(const Texture2D& diffuse, const Texture2D& normalMap,
	                               float specIntensity, float specExponent, const glm::vec4& shade)
	    : m_uniformBuffer(BufferAllocator::GetInstance().AllocateUnique(uniformBufferSize, uniformBufferFlags)),
	      m_uniformBufferMemory(reinterpret_cast<float*>(glMapNamedBufferRange(*m_uniformBuffer, 0, uniformBufferSize,
	                                                     uniformBufferFlags | GL_MAP_FLUSH_EXPLICIT_BIT))),
	      m_shade(shade), m_specularIntensity(specIntensity), m_specularExponent(specExponent),
	      m_diffuse(diffuse), m_normalMap(normalMap) { }
	
	void SpriteMaterial::Bind() const
	{
		if (m_ubNeedsUpload)
		{
			m_uniformBufferMemory[0] = m_shade[0];
			m_uniformBufferMemory[1] = m_shade[1];
			m_uniformBufferMemory[2] = m_shade[2];
			m_uniformBufferMemory[3] = m_shade[3];
			m_uniformBufferMemory[4] = m_specularIntensity;
			m_uniformBufferMemory[5] = m_specularExponent;
			
			glFlushMappedNamedBufferRange(*m_uniformBuffer, 0, uniformBufferSize);
			
			m_ubNeedsUpload = false;
		}
		
		glBindBufferBase(GL_UNIFORM_BUFFER, MATERIAL_BUFFER_BINDING, *m_uniformBuffer);
		
		glBindTextureUnit(DIFFUSE_TEXTURE_UNIT, m_diffuse.GetID());
		glBindTextureUnit(NORMAL_MAP_TEXTURE_UNIT, m_normalMap.GetID());
	}
	
	void SpriteMaterial::SetSpecularIntensity(float specularIntensity)
	{
		m_specularIntensity = specularIntensity;
		m_ubNeedsUpload = true;
	}
	
	void SpriteMaterial::SetSpecularExponent(float specularExponent)
	{
		m_specularExponent = specularExponent;
		m_ubNeedsUpload = true;
	}
	
	void SpriteMaterial::SetShade(const glm::vec4& shade)
	{
		m_shade = shade;
		m_ubNeedsUpload = true;
	}
}
