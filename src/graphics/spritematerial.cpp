#include "spritematerial.h"
#include "gl/texture2d.h"

namespace TankGame
{
	SpriteMaterial::SpriteMaterial(const Texture2D& diffuse, const Texture2D& normalMap,
	                               float specIntensity, float specExponent)
	    : SpriteMaterial(diffuse, normalMap, specIntensity, specExponent, glm::vec4(1.0f)) { }
	
	SpriteMaterial::SpriteMaterial(const Texture2D& diffuse, const Texture2D& normalMap,
	                               float specIntensity, float specExponent, const glm::vec4& shade)
	    : m_uniformBuffer(BufferAllocator::GetInstance().AllocateUnique(sizeof(float) * 3, GL_MAP_WRITE_BIT)),
	      m_specularIntensity(specIntensity), m_specularExponent(specExponent), m_shade(shade),
	      m_diffuse(diffuse), m_normalMap(normalMap) { }
	
	void SpriteMaterial::Bind() const
	{
		if (m_ubNeedsUpload)
		{
			void* ubMemory = glMapNamedBufferRange(*m_uniformBuffer, 0, sizeof(float) * 2,
			                                       GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
			
			reinterpret_cast<float*>(ubMemory)[0] = m_shade[0];
			reinterpret_cast<float*>(ubMemory)[1] = m_shade[1];
			reinterpret_cast<float*>(ubMemory)[2] = m_shade[2];
			reinterpret_cast<float*>(ubMemory)[3] = m_shade[3];
			reinterpret_cast<float*>(ubMemory)[4] = m_specularIntensity;
			reinterpret_cast<float*>(ubMemory)[5] = m_specularExponent;
			
			glUnmapNamedBuffer(*m_uniformBuffer);
			
			m_ubNeedsUpload = false;
		}
		
		glBindBufferBase(GL_UNIFORM_BUFFER, MATERIAL_BUFFER_BINDING, *m_uniformBuffer);
		
		glBindTextureUnit(DIFFUSE_TEXTURE_UNIT, m_diffuse.GetID());
		glBindTextureUnit(NORMAL_MAP_TEXTURE_UNIT, m_normalMap.GetID());
	}
}
