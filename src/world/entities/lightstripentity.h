#pragma once

#include "../entity.h"
#include "../path.h"
#include "../../utils/memory/stackobject.h"
#include "../../graphics/gl/buffer.h"
#include "../../graphics/gl/shaderprogram.h"
#include "../../graphics/gl/vertexarray.h"
#include "../../graphics/gl/bufferallocator.h"
#include "../../editor/ieditablepathprovider.h"

#include <glm/glm.hpp>

namespace TankGame
{
	class LightStripEntity : public Entity, public Entity::ISpriteDrawable, public IEditablePathProvider
	{
	public:
		inline LightStripEntity()
		    : LightStripEntity({ 1, 1, 1 }, 5, 0.5f) { }
		
		LightStripEntity(glm::vec3 color, float glowStrength, float radius);
		LightStripEntity(glm::vec3 color, float glowStrength, float radius,
		                 std::string activateEventName, glm::vec3 activatedColor);
		
		virtual void Draw(class SpriteRenderList& spriteRenderList) const override;
		
		virtual Circle GetBoundingCircle() const override;
		virtual const Entity::ISpriteDrawable* AsSpriteDrawable() const final override
		{ return this; }
		
		virtual void OnSpawned(class GameWorld& gameWorld) override;
		
		void SetColor(glm::vec3 color);
		inline glm::vec3 GetColor() const
		{ return m_color; }
		
		void SetGlowStrength(float glowStrength);
		inline float GetGlowStrength() const
		{ return m_glowStrength; }
		
		void SetRadius(float radius);
		inline float GetRadius() const
		{ return m_radius; }
		
		inline void SetActivatedColor(glm::vec3 activatedColor)
		{ m_activatedColor = activatedColor; }
		inline glm::vec3 GetActivatedColor() const
		{ return m_activatedColor; }
		
		inline void SetActivateEventName(std::string activateEvent)
		{ m_activateEvent = std::move(activateEvent); }
		
		void SetPath(Path path);
		
		virtual void HandleEvent(const std::string &event, Entity* sender) override;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		
		virtual Path& GetEditPath() override;
		virtual void PathEditEnd() override;
		virtual bool IsClosedPath() const override;
		virtual const char* GetEditPathName() const override;
		
	private:
		void SpawnLights(const Path& path);
		void PathChanged();
		
		static void BindShader();
		
		static StackObject<ShaderProgram> s_shader;
		
		Path m_path;
		glm::vec2 m_centerPath{ 0.0f, 0.0f };
		
		std::vector<class RayLightEntity*> m_lights;
		
		StackObject<Buffer> m_vertexBuffer;
		StackObject<Buffer> m_indexBuffer;
		VertexArray m_vertexArray;
		
		GLsizei m_numIndices = 0;
		
		BufferAllocator::UniquePtr m_uniformBuffer;
		
		Circle m_boundingCircle;
		
		float m_radius;
		
		std::string m_activateEvent;
		glm::vec3 m_activatedColor;
		
		glm::vec3 m_color;
		float m_glowStrength;
		mutable bool m_uniformBufferOutOfDate = true;
	};
}
