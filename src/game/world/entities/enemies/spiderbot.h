#pragma once

#include "../hittable.h"
#include "../../lights/pointlightentity.h"
#include "../../path/path.h"
#include "../../../editor/ieditoruientity.h"

namespace TankGame
{
	class SpiderBot : public PointLightEntity, public Entity::IUpdateable, public Entity::ISpriteDrawable,
	        public IEditorUIEntity, public Hittable
	{
	public:
		SpiderBot();
		
		virtual void Draw(class SpriteRenderList& spriteRenderList) const override;
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual void OnSpawned(class GameWorld& gameWorld) override;
		
		virtual Circle GetHitCircle() const override;
		
		virtual void DrawEditorUI(UIRenderer& uiRenderer, const glm::mat3& uiViewMatrix) const override;
		
		virtual const Entity::ISpriteDrawable* AsSpriteDrawable() const final override
		{ return this; }
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		virtual Hittable* AsHittable() final override
		{ return this; }
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
	protected:
		virtual void OnKilled() override;
		
	private:
		bool m_isAwake = false;
		
		glm::vec2 m_legSize;
		
		const class PropClass& m_bodyPropClass;
		const class PropClass& m_legsPropClass;
		
		Path m_pathToPlayer;
		float m_pathToPlayerProgress = 0;
		double m_pathToPlayerNextUpdateTime = 0;
		
		float m_animationTime = 0.25f;
		
		float m_wakeupAnimationTime = 0;
		
		class PlayerEntity* m_player = nullptr;
	};
}
