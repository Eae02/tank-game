#pragma once

#include <string>

#include "../entity.h"

namespace TankGame
{
	class PropEntity : public virtual Entity, public Entity::ISpriteDrawable
	{
	public:
		explicit PropEntity(const std::string& propClassName, bool isSolid = false);
		
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
		
		virtual const Entity::ISpriteDrawable* AsSpriteDrawable() const final override
		{ return this; }
		
		virtual IntersectInfo GetIntersectInfo(const Circle& circle) const override;
		
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
