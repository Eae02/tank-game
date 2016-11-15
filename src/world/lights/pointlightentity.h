#pragma once

#include "lightsourceentity.h"

namespace TankGame
{
	class PointLightEntity : public LightSourceEntity
	{
	public:
		inline PointLightEntity()
		    : PointLightEntity({ 1.0f, 1.0f, 1.0f }, 3.0f, { }) { }
		PointLightEntity(glm::vec3 color, float intensity, Attenuation attenuation, float height = 0.5f);
		
		virtual const ShaderProgram& GetShader() const final override;
		
		virtual const char* GetSerializeClassName() const;
		
		virtual const char* GetObjectName() const override;
		virtual void RenderProperties() override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
	protected:
		virtual int GetPositionUniformLocation() const final override
		{ return s_positionUniformLocation; }
		virtual int GetWorldTransformUniformLocation() const final override
		{ return s_worldTransformUniformLocation; }
		
	private:
		static StackObject<ShaderProgram> s_shaderProgram;
		
		static int s_positionUniformLocation;
		static int s_worldTransformUniformLocation;
	};
}
