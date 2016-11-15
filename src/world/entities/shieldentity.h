#pragma once

#include "../../utils/memory/stackobject.h"
#include "../../graphics/gl/shaderprogram.h"
#include "../../graphics/gl/bufferallocator.h"
#include "../../graphics/gl/vertexarray.h"
#include "../entity.h"

namespace TankGame
{
	class ShieldEntity : public Entity, public Entity::ISpriteDrawable,
	        public Entity::IDistortionDrawable, public Entity::IUpdateable
	{
	public:
		ShieldEntity();
		
		virtual Circle GetBoundingCircle() const override;
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual void Draw(class SpriteRenderList& spriteRenderList) const override;
		
		virtual void DrawDistortions() const override;
		
		virtual void OnSpawned(class GameWorld& gameWorld) override;
		
		virtual const Entity::IDistortionDrawable* AsDistortionDrawable() const final override
		{ return this; }
		virtual const Entity::ISpriteDrawable*AsSpriteDrawable() const final override
		{ return this; }
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		
		inline void SetRadius(float radius)
		{ m_radius = radius; }
		inline float GetRadius() const
		{ return m_radius; }
		
	private:
		static StackObject<ShaderProgram> s_distortionShader;
		
		static StackObject<Buffer> s_vertexBuffer;
		static StackObject<VertexArray> s_vertexArray;
		static GLsizei s_numVertices;
		
		static void CreateVertexBuffer();
		
		BufferAllocator::UniquePtr m_settingsBuffer;
		
		float m_radius = 1;
	};
}
