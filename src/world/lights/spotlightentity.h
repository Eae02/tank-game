#pragma once

#include "lightsourceentity.h"
#include <glm/gtc/constants.hpp>

namespace TankGame
{
	class SpotLightEntity : public LightSourceEntity
	{
	public:
		inline SpotLightEntity()
		    : SpotLightEntity({ 1.0f, 1.0f, 1.0f }, 3.0f, glm::half_pi<float>(), {}) { }
		
		SpotLightEntity(glm::vec3 color, float intensity, float cutoffAngle,
		                Attenuation attenuation, float height = 0.5f);
		
		virtual const ShaderProgram& GetShader() const final override;
		virtual void Bind() const final override;
		
		inline float GetCutoffAngle() const
		{ return m_cutoffAngle; }
		void SetCutoffAngle(float cutoffAngle);
		
		virtual const char* GetObjectName() const override;
		virtual void RenderProperties() override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
	protected:
		virtual void UpdateUniformBuffer(void* memory) const final override;
		
		virtual int GetPositionUniformLocation() const final override
		{ return s_positionUniformLocation; }
		virtual int GetWorldTransformUniformLocation() const final override
		{ return s_worldTransformUniformLocation; }
		
	private:
		static std::unique_ptr<ShaderProgram> s_shaderProgram;
		
		static int s_positionUniformLocation;
		static int s_directionUniformLocation;
		static int s_worldTransformUniformLocation;
		
		float m_cutoff;
		float m_cutoffAngle;
	};
}
