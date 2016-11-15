#pragma once

#include "gl/bufferallocator.h"
#include <glm/glm.hpp>

namespace TankGame
{
	class SpriteMaterial
	{
	public:
		SpriteMaterial(const class Texture2D& diffuse, const class Texture2D& normalMap,
		               float specIntensity, float specExponent);
		
		SpriteMaterial(const class Texture2D& diffuse, const class Texture2D& normalMap,
		               float specIntensity, float specExponent, const glm::vec4& shade);
		
		void Bind() const;
		
		inline float GetSpecularIntensity() const
		{ return m_specularIntensity; }
		inline void SetSpecularIntensity(float specularIntensity)
		{
			m_specularIntensity = specularIntensity;
			m_ubNeedsUpload = true;
		}
		
		inline float GetSpecularExponent() const
		{ return m_specularExponent; }
		void SetSpecularExponent(float specularExponent)
		{
			m_specularExponent = specularExponent;
			m_ubNeedsUpload = true;
		}
		
		inline const glm::vec4& GetShade() const
		{ return m_shade; }
		void SetShade(const glm::vec4& shade)
		{
			m_shade = shade;
			m_ubNeedsUpload = true;
		}
		
		static constexpr int MATERIAL_BUFFER_BINDING = 1;
		
		static constexpr int DIFFUSE_TEXTURE_UNIT = 0;
		static constexpr int NORMAL_MAP_TEXTURE_UNIT = 1;
		
	private:
		BufferAllocator::UniquePtr m_uniformBuffer;
		mutable bool m_ubNeedsUpload = true;
		
		glm::vec4 m_shade;
		float m_specularIntensity;
		float m_specularExponent;
		
		const class Texture2D& m_diffuse;
		const class Texture2D& m_normalMap;
	};
}
