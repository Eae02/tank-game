#pragma once

#include "../entity.h"
#include "../path/path.h"
#include "../../graphics/gl/buffer.h"
#include "../../graphics/gl/shaderprogram.h"
#include "../../graphics/gl/vertexinputstate.h"
#include "../../graphics/gl/bufferallocator.h"
#include "../../editor/ieditablepathprovider.h"


namespace TankGame
{
	class LightStripEntity : public Entity, public Entity::ISpriteDrawable, public IEditablePathProvider
	{
	public:
		LightStripEntity()
		    : LightStripEntity({ 1, 1, 1 }, 5, 0.5f) { }
		
		LightStripEntity(glm::vec3 color, float glowStrength, float radius)
			: m_radius(radius), m_color(color), m_glowStrength(glowStrength) { }
		
		virtual void Draw(class SpriteRenderList& spriteRenderList) const override;
		
		virtual Circle GetBoundingCircle() const override;
		virtual const Entity::ISpriteDrawable* AsSpriteDrawable() const final override
		{ return this; }
		
		virtual void OnSpawned(class GameWorld& gameWorld) override;
		
		virtual void OnDespawning() override;
		
		void SetColor(glm::vec3 color);
		inline glm::vec3 GetColor() const
		{ return m_color; }
		
		void SetGlowStrength(float glowStrength);
		inline float GetGlowStrength() const
		{ return m_glowStrength; }
		
		void SetRadius(float radius);
		inline float GetRadius() const
		{ return m_radius; }
		
		void SetPath(Path path);
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
		virtual void EditorMoved() override;
		virtual void EditorSpawned() override;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		
		virtual Path& GetEditPath() override;
		virtual void PathEditEnd() override;
		virtual bool IsClosedPath() const override;
		virtual const char* GetEditPathName() const override;
		
	protected:
		virtual void PushLuaMetaTable(lua_State* state) const override;
		
	private:
		static Lua::RegistryReference s_metaTableRef;
		
		void SpawnLights(const Path& path);
		void PathChanged();
		
		static void BindShader();
		
		static std::unique_ptr<ShaderProgram> s_shader;
		static int s_colorUniformLoc;
		
		Path m_path;
		glm::vec2 m_centerPath{ 0.0f, 0.0f };
		
		std::vector<class RayLightEntity*> m_lights;
		
		std::unique_ptr<Buffer> m_vertexBuffer;
		std::unique_ptr<Buffer> m_indexBuffer;
		VertexInputState m_vertexInputState;
		
		GLsizei m_numIndices = 0;
		
		Circle m_boundingCircle;
		
		float m_radius;
		
		glm::vec3 m_color;
		float m_glowStrength;
	};
}
