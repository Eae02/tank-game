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
		
		virtual void HandleEvent(const std::string& event, Entity* sender) override;
		
		virtual Path& GetEditPath() override;
		virtual void PathEditEnd() override;
		virtual bool IsClosedPath() const override
		{ return true; }
		virtual const char* GetEditPathName() const override;
		
		virtual void RenderProperties() override;
		
		inline void SetDetectPlayerEventName(std::string eventName)
		{ m_detectPlayerEventName = std::move(eventName); }
		inline const std::string& GetDetectPlayerEventName() const
		{ return m_detectPlayerEventName; }
		
		inline void SetOnKilledEventName(std::string eventName)
		{ m_onKilledEventName = eventName; }
		inline const std::string& GetOnKilledEventName() const
		{ return m_onKilledEventName; }
		
		inline bool HasShield() const
		{ return m_hasShield; }
		inline void SetHasShield(bool hasShield)
		{ m_hasShield = hasShield; }
		
	protected:
		virtual const class SpriteMaterial& GetBaseMaterial(int frame) const override;
		virtual const class SpriteMaterial& GetCannonMaterial() const override;
		
		virtual void OnKilled() override;
		
	private:
		static bool s_areTexturesLoaded;
		static std::unique_ptr<Texture2D> s_cannonTexture;
		static std::unique_ptr<Texture2D> s_cannonNormalMap;
		static std::unique_ptr<SpriteMaterial> s_cannonMaterial;
		
		bool m_isRocketTank = false;
		bool m_hasShield = false;
		
		std::string m_onKilledEventName;
		std::string m_detectPlayerEventName;
		
		EnemyAI m_ai;
		
		glm::vec2 m_oldPosition;
	};
}
