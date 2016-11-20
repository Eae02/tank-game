#pragma once

#include "intersectinfo.h"
#include "quadtree/iquadtreeentry.h"
#include "itransformationprovider.h"
#include "../editor/properties/ipropertiesobject.h"
#include "../transform.h"
#include "../utils/abstract.h"

#include <json.hpp>
#include <glm/glm.hpp>

namespace TankGame
{
	class Entity : public IQuadTreeEntry, public ITransformationProvider, public IPropertiesObject
	{
	public:
		enum class SolidTypes
		{
			Object,
			Npc,
			Player
		};
		
		class IUpdateable : public Abstract
		{
		public:
			virtual void Update(const class UpdateInfo& updateInfo) = 0;
		};
		
		class ISpriteDrawable : public Abstract
		{
		public:
			virtual void Draw(class SpriteRenderList& spriteRenderList) const = 0;
		};
		
		class ITranslucentSpriteDrawable : public Abstract
		{
		public:
			virtual void DrawTranslucent(class SpriteRenderList& spriteRenderList) const = 0;
		};
		
		class IDistortionDrawable : public Abstract
		{
		public:
			virtual void DrawDistortions() const = 0;
		};
		
		virtual Rectangle GetBoundingRectangle() const final override;
		
		virtual Circle GetBoundingCircle() const;
		
		void Despawn();
		
		inline SolidTypes GetSolidType() const
		{ return m_solidType; }
		
		virtual const Transform& GetTransform() const final override
		{ return m_transform; }
		inline Transform& GetTransform()
		{ return m_transform; }
		
		inline class GameWorld* GetGameWorld()
		{ return m_world; }
		inline const class GameWorld* GetGameWorld() const
		{ return m_world; }
		
		virtual bool CanInteract() const
		{ return false; }
		virtual void OnInteract() { }
		
		virtual void OnSpawned(class GameWorld& gameWorld);
		virtual void OnDespawning() { }
		
		virtual const ISpriteDrawable* AsSpriteDrawable() const
		{ return nullptr; }
		virtual const ITranslucentSpriteDrawable* AsTranslucentSpriteDrawable() const
		{ return nullptr; }
		virtual const IDistortionDrawable* AsDistortionDrawable() const
		{ return nullptr; }
		virtual IUpdateable* AsUpdatable()
		{ return nullptr; }
		
		virtual const class ILightSource* AsLightSource() const
		{ return nullptr; }
		virtual class Hittable* AsHittable()
		{ return nullptr; }
		
		virtual IntersectInfo GetIntersectInfo(const Circle& circle) const
		{ return { }; }
		
		inline void SetEditorVisible(bool editorVisible)
		{ m_editorVisible = editorVisible; }
		inline bool IsEditorVisible() const
		{ return m_editorVisible; }
		
		virtual const char* GetSerializeClassName() const
		{ return nullptr; }
		virtual nlohmann::json Serialize() const;
		
		virtual void HandleEvent(const std::string& event, Entity* sender) { }
		
		virtual std::unique_ptr<Entity> Clone() const;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
	protected:
		inline void SetSolidType(SolidTypes solidType)
		{ m_solidType = solidType; }
		
		inline void RenderTransformProperty()
		{ RenderTransformProperty(Transform::Properties::All); }
		void RenderTransformProperty(Transform::Properties propertiesToShow);
		
	private:
		SolidTypes m_solidType = SolidTypes::Object;
		
		class GameWorld* m_world = nullptr;
		
		Transform m_transform;
		
		bool m_editorVisible = true;
	};
}
