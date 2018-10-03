#pragma once

#include <string>

#include "../entity.h"
#include "../icollidable.h"

namespace TankGame
{
	class PropEntity : public virtual Entity, public ICollidable, public Entity::ITranslucentSpriteDrawable,
	        public Entity::ISpriteDrawable
	{
	public:
		explicit PropEntity(const std::string& propClassName, float sizeX = 0.5f, bool isSolid = false);
		
		virtual void DrawTranslucent(class SpriteRenderList& spriteRenderList) const override;
		virtual void Draw(class SpriteRenderList& spriteRenderList) const override;
		
		void SetSizeX(float sizeX);
		inline float GetSizeX() const
		{ return m_sizeX; }
		
		inline void SetZ(float z)
		{ m_z = z; }
		inline float GetZ() const
		{ return m_z; }
		
		inline void SetIsSolid(bool isSolid)
		{ m_isSolid = isSolid; }
		inline bool IsSolid() const
		{ return m_isSolid; }
		
		virtual const Entity::ITranslucentSpriteDrawable* AsTranslucentSpriteDrawable() const final override
		{ return this; }
		virtual const Entity::ISpriteDrawable* AsSpriteDrawable() const final override
		{ return this; }
		virtual const ICollidable* AsCollidable() const final override
		{ return this; }
		
		virtual ColliderInfo GetColliderInfo() const override;
		virtual CollidableTypes GetCollidableType() const override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
	private:
		const class PropClass* m_propClass;
		bool m_isSolid;
		float m_aspectRatio;
		float m_z = 0.9f;
		float m_sizeX;
	};
}
