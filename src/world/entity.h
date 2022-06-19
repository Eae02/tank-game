#pragma once

#include "intersectinfo.h"
#include "quadtree/iquadtreeentry.h"
#include "itransformationprovider.h"
#include "../editor/properties/ipropertiesobject.h"
#include "../transform.h"
#include "../utils/abstract.h"
#include "../colliderinfo.h"
#include "../lua/registryreference.h"
#include "../lua/luainc.h"

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>

namespace TankGame
{
	class Entity : public IQuadTreeEntry, public ITransformationProvider, public IPropertiesObject
	{
	public:
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
		
		virtual const Transform& GetTransform() const final override
		{ return m_transform; }
		inline Transform& GetTransform()
		{ return m_transform; }
		
		inline class GameWorld* GetGameWorld() const
		{ return m_world; }
		
		virtual bool CanInteract()
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
		virtual const class ICollidable* AsCollidable() const
		{ return nullptr; }
		virtual IUpdateable* AsUpdatable()
		{ return nullptr; }
		
		virtual const class ILightSource* AsLightSource() const
		{ return nullptr; }
		virtual class Hittable* AsHittable()
		{ return nullptr; }
		
		inline void SetEditorVisible(bool editorVisible)
		{ m_editorVisible = editorVisible; }
		inline bool IsEditorVisible() const
		{ return m_editorVisible; }
		
		virtual const char* GetSerializeClassName() const
		{ return nullptr; }
		virtual nlohmann::json Serialize() const;
		
		virtual void EditorMoved() { }
		virtual void EditorSpawned() { }
		
		virtual std::unique_ptr<Entity> Clone() const;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
		inline const std::string& GetName() const
		{ return m_name; }
		inline void SetName(std::string name)
		{ m_name = std::move(name); }
		
		void PushLuaInstance(lua_State* state);
		
	protected:
		virtual void PushLuaMetaTable(lua_State* state) const;
		
		inline void RenderBaseProperties()
		{ RenderBaseProperties(Transform::Properties::All); }
		void RenderBaseProperties(Transform::Properties propertiesToShow, bool nameable = true);
		
		static Entity* LuaGetInstance(lua_State* state, bool errorOnFail = true);
		static void NewLuaMetaTable(lua_State* state);
		
	private:
		static Lua::RegistryReference s_metaTableRef;
		
		std::string m_name;
		
		Lua::RegistryReference m_luaObject;
		
		class GameWorld* m_world = nullptr;
		
		Transform m_transform;
		
		bool m_editorVisible = true;
	};
}
