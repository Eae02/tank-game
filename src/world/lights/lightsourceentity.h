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
	class LightSourceEntity : public virtual Entity, public IShadowLightSource
	{
	public:
		virtual Circle GetBoundingCircle() const override;
		
		virtual const ShaderProgram& GetShader() const override = 0;
		virtual void Bind() const override;
		
		virtual LightInfo GetLightInfo() const override;
		
		inline const glm::vec3& GetColor() const
		{ return m_color; }
		void SetColor(const glm::vec3& color);
		
		inline float GetIntensity() const
		{ return m_intensity; }
		void SetIntensity(float intensity);
		
		inline const Attenuation& GetAttenuation() const
		{ return m_attenuation; }
		void SetAttenuation(const Attenuation& attenuation);
		
		inline float GetHeight() const
		{ return m_height; }
		inline void SetHeight(float height)
		{ m_height = height; }
		
		void SetFlickers(bool flickers);
		inline bool Flickers() const
		{ return m_flickers; }
		
		virtual ShadowMap* GetShadowMap() const override;
		void SetShadowMode(EntityShadowModes mode);
		inline EntityShadowModes GetShadowMode() const
		{ return m_shadowMode; }
		
		virtual void InvalidateShadowMap() override;
		
		virtual void OnSpawned(class GameWorld& gameWorld) override;
		
		virtual const ILightSource* AsLightSource() const final override
		{ return this; }
		
		virtual nlohmann::json Serialize() const;
		
		virtual void HandleEvent(const std::string& event, Entity* sender) override;
		
	protected:
		LightSourceEntity(glm::vec3 color, float intensity, Attenuation attenuation, float height, size_t ubSize);
		
		virtual void UpdateUniformBuffer(void* memory) const;
		
		virtual int GetPositionUniformLocation() const = 0;
		virtual int GetWorldTransformUniformLocation() const = 0;
		
		inline void InvalidateUniformBuffer()
		{ m_uniformBufferOutOfDate = true; }
		
		void RenderLightSourceProperties();
		
		inline float GetFlickerOffset() const
		{ return m_flickerOffset; }
		
	private:
		size_t m_ubSize;
		
		bool m_flickers = false;
		float m_flickerOffset;
		
		mutable StackObject<ShadowMap> m_shadowMap;
		EntityShadowModes m_shadowMode = EntityShadowModes::None;
		
		BufferAllocator::UniquePtr m_uniformBuffer;
		mutable bool m_uniformBufferOutOfDate = true;
		
		BufferAllocator::UniquePtr m_flickerUniformBuffer;
		
		glm::vec3 m_color;
		float m_intensity;
		Attenuation m_attenuation;
		
		float m_height;
		
		float m_range;
	};
}
