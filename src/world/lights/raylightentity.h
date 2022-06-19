#pragma once

#include "../entity.h"
#include "ilightsource.h"
#include "attenuation.h"
#include "ishadowlightsource.h"

#include "../../graphics/gl/bufferallocator.h"
#include "../../graphics/shadowmap.h"
#include "../../graphics/gl/buffer.h"
#include "../../graphics/gl/shaderprogram.h"

namespace TankGame
{
	class RayLightEntity : public virtual Entity, public ILightSource
	{
	public:
		inline RayLightEntity()
		    : RayLightEntity({ 1.0f, 1.0f, 1.0f }, 3.0f, { }, 1.0f) { }
		
		RayLightEntity(glm::vec3 color, float intensity, Attenuation attenuation, float length, float height = 0.5f);
		
		virtual Circle GetBoundingCircle() const override;
		
		inline const glm::vec3& GetColor() const
		{ return m_color; }
		
		void SetColor(const glm::vec3& color);
		
		inline float GetIntensity() const
		{ return m_intensity; }
		
		void SetIntensity(float intensity);
		
		inline const Attenuation& GetAttenuation() const
		{ return m_attenuation; }
		
		void SetAttenuation(const Attenuation& attenuation);
		
		inline void SetLength(float length)
		{ m_length = length; }
		inline float GetLength() const
		{ return m_length; }
		
		void SetFlickerIntensity(float flickerIntensity);
		
		virtual const ShaderProgram& GetShader() const override;
		virtual void Bind() const override;
		virtual LightInfo GetLightInfo() const override;
		
		virtual const ILightSource* AsLightSource() const final override
		{ return this; }
		
		glm::mat3 GetWorldTransform() const;
		
	private:
		static std::unique_ptr<ShaderProgram> s_shaderProgram;
		
		static int s_positionUniformLocation;
		static int s_directionUniformLocation;
		static int s_heightUniformLocation;
		static int s_worldTransformUniformLocation;
		
		BufferAllocator::UniquePtr m_uniformBuffer;
		mutable bool m_uniformBufferOutOfDate = true;
		
		float m_flickerOffset;
		float m_flickerIntensity = 0.0f;
		
		float m_height;
		
		float m_range;
		
		float m_length;
		
		glm::vec3 m_color;
		float m_intensity;
		Attenuation m_attenuation;
	};
}
