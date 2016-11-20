#pragma once

#include "../entity.h"
#include "../../utils/memory/stackobject.h"
#include "../../graphics/gl/texture2d.h"
#include "../../graphics/gl/shaderprogram.h"
#include "../../graphics/gl/bufferallocator.h"

namespace TankGame
{
	class ConveyorBeltEntity : public Entity, public Entity::IUpdateable, public Entity::ISpriteDrawable
	{
	public:
		ConveyorBeltEntity(float speed);
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual void Draw(class SpriteRenderList& spriteRenderList) const override;
		
		virtual const Entity::ISpriteDrawable* AsSpriteDrawable() const final override
		{ return this; }
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		
		virtual void HandleEvent(const std::string& event, Entity* sender) override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
		inline void SetSpeed(float speed)
		{ m_speed = speed; }
		inline float GetSpeed() const
		{ return m_speed; }
		
		inline void SetEnabled(bool enabled)
		{ m_enabled = enabled; }
		inline bool IsEnabled() const
		{ return m_enabled; }
		
		glm::vec2 GetPushVector(glm::vec2 position) const;
		
		static void LoadResources(class ASyncWorkList& workList);
		
	private:
		static StackObject<Texture2D> s_diffuseTexture;
		static StackObject<Texture2D> s_normalMap;
		static StackObject<Texture2D> s_specularTexture;
		
		static StackObject<ShaderProgram> s_shader;
		
		BufferAllocator::UniquePtr m_uniformBuffer;
		
		float m_speed;
		bool m_enabled = true;
		
		float m_textureOffset = 0;
	};
}
