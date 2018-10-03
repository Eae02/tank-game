#pragma once

#include "propentity.h"

namespace TankGame
{
	class DoorEntity : public PropEntity
	{
	public:
		DoorEntity();
		
		void Open();
		
		virtual void RenderProperties() override;
		virtual const char*GetObjectName() const override;
		
		virtual nlohmann::json Serialize() const override;
		virtual const char* GetSerializeClassName() const override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
	protected:
		virtual void PushLuaMetaTable(lua_State* state) const override;
		
	private:
		static Lua::RegistryReference s_metaTableRef;
	};
}
