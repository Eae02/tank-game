#pragma once

#include "propentity.h"

namespace TankGame
{
	class ActivatorEntity : public PropEntity
	{
	public:
		ActivatorEntity(std::string onActivateEvent = "", bool singleUse = true);
		
		virtual bool CanInteract() const override;
		virtual void OnInteract() override;
		
		virtual const char* GetObjectName() const override;
		virtual void RenderProperties() override;
		
		virtual nlohmann::json Serialize() const override;
		virtual const char* GetSerializeClassName() const override;
		
		virtual std::unique_ptr<Entity> Clone() const override;
		
	private:
		std::string m_onActivateEvent;
		bool m_singleUse;
		
		bool m_hasActivated = false;
	};
}
