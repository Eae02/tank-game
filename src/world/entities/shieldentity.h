#pragma once

#include "../../graphics/gl/shaderprogram.h"
#include "../../graphics/gl/bufferallocator.h"
#include "../../graphics/gl/vertexinputstate.h"
#include "../entity.h"
#include "hittable.h"

#include <memory>

namespace TankGame
{
	class ShieldEntity : public Entity, public Hittable, public Entity::ITranslucentSpriteDrawable,
	        public Entity::IDistortionDrawable, public Entity::IUpdateable
	{
	public:
		ShieldEntity(float hp, int teamID, float radius);
		
		virtual Circle GetBoundingCircle() const override;
		
		virtual Circle GetHitCircle() const override;
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual void DrawTranslucent(class SpriteRenderList& spriteRenderList) const override;
		
		virtual void DrawDistortions() const override;
		
		virtual const Entity::IDistortionDrawable* AsDistortionDrawable() const final override
		{ return this; }
		virtual const Entity::ITranslucentSpriteDrawable* AsTranslucentSpriteDrawable() const final override
		{ return this; }
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		virtual Hittable* AsHittable() final override
		{ return this; }
		
		inline void SetRadius(float radius)
		{ m_radius = radius; }
		inline float GetRadius() const
		{ return m_radius; }
		
		void Ripple(float originAngle);
		
		virtual bool CanMoveDuringUpdate() const override { return true; }
		
	protected:
		virtual void OnKilled() override;
		
	private:
		static std::unique_ptr<ShaderProgram> s_distortionShader;
		static std::unique_ptr<ShaderProgram> s_spriteShader;
		
		static std::unique_ptr<Buffer> s_vertexBuffer;
		static std::unique_ptr<VertexInputState> s_vertexInputState;
		static GLsizei s_numVertices;
		
		static void CreateVertexBuffer();
		
		BufferAllocator::UniquePtr m_settingsBuffer;
		
		static constexpr float RIPPLE_TIME = 1;
		
		float m_rippleProgress = std::numeric_limits<float>::max();
		float m_rippleAngle = 0;
		
		float m_intensity = 0;
		
		float m_radius = 1;
	};
}
