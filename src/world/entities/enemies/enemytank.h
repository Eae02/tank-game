#pragma once

#include "../hittable.h"
#include "../tankentity.h"
#include "../../../ai/enemyai.h"
#include "../../../editor/ieditablepathprovider.h"

namespace TankGame
{
	class EnemyTank : public TankEntity, public IEditablePathProvider
	{
	public:
		explicit EnemyTank(const Path& idlePath);
		
		void SetIsRocketTank(bool isRocketTank);
		bool IsRocketTank() const
		{ return m_isRocketTank; }
		
		void DetectPlayer();
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual void OnSpawned(class GameWorld& gameWorld) override;
		
		virtual Hittable* AsHittable() final override
		{ return this; }
		
		virtual Circle GetHitCircle() const;
		
		virtual const char* GetObjectName() const override;
		
		static const class SpriteMaterial& CannonMaterial();
		
		static Transform GetBaseCannonTransform();
		
		virtual CollidableTypes GetCollidableType() const override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		
		virtual void EditorMoved() override;
		virtual void EditorSpawned() override;
		
		virtual Path& GetEditPath() override;
		virtual void PathEditEnd() override;
		virtual bool IsClosedPath() const override
		{ return true; }
		virtual const char* GetEditPathName() const override;
		
		virtual void RenderProperties() override;
		
		inline bool HasShield() const
		{ return m_hasShield; }
		inline void SetHasShield(bool hasShield)
		{ m_hasShield = hasShield; }
		
	protected:
		virtual void PushLuaMetaTable(lua_State* state) const override;
		
		virtual const class SpriteMaterial& GetBaseMaterial(int frame) const override;
		virtual const class SpriteMaterial& GetCannonMaterial() const override;
		
		virtual void OnKilled() override;
		
	private:
		static Lua::RegistryReference s_metaTableRef;
		
		static bool s_areTexturesLoaded;
		static std::unique_ptr<Texture2D> s_cannonTexture;
		static std::unique_ptr<Texture2D> s_cannonNormalMap;
		static std::unique_ptr<SpriteMaterial> s_cannonMaterial;
		
		Lua::RegistryReference m_onKilledCallback;
		
		bool m_isRocketTank = false;
		bool m_hasShield = false;
		
		EnemyAI m_ai;
		
		glm::vec2 m_oldPosition;
	};
}
