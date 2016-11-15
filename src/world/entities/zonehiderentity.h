#pragma once

#include "../entity.h"
#include "../../utils/memory/stackobject.h"
#include "../../graphics/gl/shaderprogram.h"
#include "../../editor/ieditoruientity.h"

namespace TankGame
{
	class ZoneHiderEntity : public Entity, public Entity::ISpriteDrawable, public IEditorUIEntity
	{
	public:
		explicit ZoneHiderEntity(std::string name);
		
		virtual void Draw(class SpriteRenderList& spriteRenderList) const override;
		
		virtual void DrawEditorUI(class UIRenderer& uiRenderer, const glm::mat3& uiViewMatrix) const override;
		
		virtual const Entity::ISpriteDrawable* AsSpriteDrawable() const final override
		{ return this; }
		
		virtual void OnSpawned(class GameWorld& gameWorld) override;
		
		virtual void HandleEvent(const std::string& event, Entity* sender);
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
	private:
		static StackObject<ShaderProgram> s_shader;
		static int s_transformUniformLocation;
		
		bool m_hidden = false;
		
		std::string m_name;
		
		std::string m_showEvent;
		std::string m_hideEvent;
	};
}
