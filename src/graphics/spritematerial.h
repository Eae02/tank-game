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
		void SetSpecularIntensity(float specularIntensity);
		
		inline float GetSpecularExponent() const
		{ return m_specularExponent; }
		void SetSpecularExponent(float specularExponent);
		
		inline const glm::vec4& GetShade() const
		{ return m_shade; }
		void SetShade(const glm::vec4& shade);
		
		static constexpr int MATERIAL_BUFFER_BINDING = 1;
		
		static constexpr int DIFFUSE_TEXTURE_UNIT = 0;
		static constexpr int NORMAL_MAP_TEXTURE_UNIT = 1;
		
	private:
		BufferAllocator::UniquePtr m_uniformBuffer;
		float* m_uniformBufferMemory;
		mutable bool m_ubNeedsUpload = true;
		
		glm::vec4 m_shade;
		float m_specularIntensity;
		float m_specularExponent;
		
		const class Texture2D& m_diffuse;
		const class Texture2D& m_normalMap;
	};
}
