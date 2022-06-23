#pragma once

#include "../entity.h"
#include "../entityhandle.h"

namespace TankGame
{
	//A group of flame throwers emitting in all directions. Currently unused.
	class OmniFlameThrower : public Entity, public Entity::IUpdateable
	{
	public:
		OmniFlameThrower(int count, float flameLength);
		
		void SetEnabled(bool enabled);
		inline bool IsEnabled() const
		{ return m_enabled; }
		
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		
		virtual void Update(const UpdateInfo& updateInfo) override;
		
		virtual void OnSpawned(GameWorld& gameWorld) override;
		virtual void OnDespawning() override;
		
		virtual void RenderProperties() override;
		virtual const char* GetObjectName() const override;
		
		virtual const char* GetSerializeClassName() const override;
		virtual nlohmann::json Serialize() const override;
		
		virtual bool CanMoveDuringUpdate() const override { return true; }
		
	protected:
		virtual void PushLuaMetaTable(lua_State* state) const override;
		
	private:
		static Lua::RegistryReference s_metaTableRef;
		
		void UpdateChildEntityTransforms();
		
		std::vector<EntityHandle> m_flameThrowers;
		
		bool m_enabled = false;
		
		float m_oldRotation;
		
		int m_count;
		float m_flameLength;
	};
}
