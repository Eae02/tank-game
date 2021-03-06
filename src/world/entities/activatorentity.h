#pragma once

#include "propentity.h"

namespace TankGame
{
	class ActivatorEntity : public PropEntity
	{
	public:
		explicit ActivatorEntity(std::string onActivateScript = "", bool singleUse = true);
		
		virtual bool CanInteract() override;
		virtual void OnInteract() override;
		
		virtual const char* GetObjectName() const override;
		virtual void RenderProperties() override;
		
		virtual nlohmann::json Serialize() const override;
		virtual const char* GetSerializeClassName() const override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
	protected:
		virtual void PushLuaMetaTable(lua_State* state) const override;
		
	private:
		static Lua::RegistryReference s_metaTableRef;
		
		std::string m_onActivateScript;
		
		bool m_singleUse;
		
		bool m_hasActivated = false;
	};
}
