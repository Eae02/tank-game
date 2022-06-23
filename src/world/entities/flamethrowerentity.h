#pragma once

#include "../icollidable.h"
#include "../entityhandle.h"
#include "../lights/pointlightentity.h"
#include "../../audio/audiosource.h"
#include "../../editor/ieditoruientity.h"
#include "../../graphics/ui/uirenderer.h"
#include "../../graphics/gl/shaderprogram.h"

#include <memory>

namespace TankGame
{
	class FlameThrowerEntity : public Entity, public Entity::ISpriteDrawable, public Entity::IDistortionDrawable,
	        public Entity::IUpdateable, public ICollidable, public IEditorUIEntity
	{
	public:
		FlameThrowerEntity(float flameLength, float enabledTime, float disabledTime, float startTime);
		
		static FlameThrowerEntity CreateManuallyControlled(float flameLength)
		{
			FlameThrowerEntity entity(flameLength, 0.0f, 0.0f, 0.0f);
			entity.m_manualEnableControl = true;
			return entity;
		}
		
		inline void SetEnabled(bool enabled)
		{ m_manuallyEnabled = enabled; }
		
		virtual void Update(const UpdateInfo& updateInfo) override;
		
		virtual void Draw(SpriteRenderList& spriteRenderList) const override;
		
		virtual void DrawDistortions() const override;
		
		virtual const Entity::ISpriteDrawable* AsSpriteDrawable() const final override
		{ return this; }
		
		virtual const Entity::IDistortionDrawable* AsDistortionDrawable() const final override
		{ return this; }
		
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		
		virtual const ICollidable* AsCollidable() const final override
		{ return this; }
		
		virtual void DrawEditorUI(UIRenderer& uiRenderer, const glm::mat3& uiViewMatrix) const override;
		
		virtual void OnSpawned(GameWorld& gameWorld) override;
		virtual void OnDespawning() override;
		virtual void EditorMoved() override;
		
		virtual Circle GetBoundingCircle() const override;
		
		virtual ColliderInfo GetColliderInfo() const override;
		virtual CollidableTypes GetCollidableType() const override;
		virtual bool IsStaticCollider() const override;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		virtual std::unique_ptr<Entity> Clone() const override;
		
		virtual bool CanMoveDuringUpdate() const override { return false; }
		
		inline void TransformModified()
		{
			SetChildEntitiesTransform(m_particleSystemEntity.Get(), m_lightEntity.Get());
		}
		
	protected:
		virtual void PushLuaMetaTable(lua_State* state) const override;
		
	private:
		static Lua::RegistryReference s_metaTableRef;
		
		struct DistortionShader
		{
			ShaderProgram m_shader;
			int m_transformLocation;
			int m_intensityLocation;
			
			Texture2D m_dudvMap;
			Buffer m_textureMatricesBuffer;
		};
		
		static std::unique_ptr<DistortionShader> s_distortionShader;
		
		void SpawnChildEntities();
		void SetChildEntitiesTransform(Entity* psEntity, Entity* lightEntity);
		
		//Returns the world space position of the middle of the flame.
		glm::vec2 GetMidFlamePos() const;
		
		static constexpr float Size = 0.3f;
		static constexpr float DamagePerSecond = 60.0f;
		
		AudioSource m_audioSource;
		
		float m_flameLength;
		
		bool m_psEnabled = true;
		bool m_wasEnabled = false;
		
		bool m_manualEnableControl = false;
		bool m_manuallyEnabled = false; // Only used if m_manualEnableControl is true.
		
		//Stores timing parameters, only used if m_manualEnableControl is false.
		float m_enabledTime;
		float m_disabledTime;
		float m_startTime;
		
		float m_enableProgress = 0;
		float m_timeSinceEnableSwitch = std::numeric_limits<float>::infinity();
		
		const class PropClass& m_flameThrowerPropClass;
		
		EntityHandle m_particleSystemEntity;
		EntityHandle m_lightEntity;
	};
}
