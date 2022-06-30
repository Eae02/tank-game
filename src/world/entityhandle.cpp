#include "entityhandle.h"
#include "entitiesmanager.h"

namespace TankGame
{
	/*
	EntityHandle::EntityHandle(EntitiesManager& manager, const Entity& entity)
	{
		auto pos = std::find_if(manager.m_entities.begin(), manager.m_entities.end(), [&] (const auto& e)
		{
			return e.m_entity.get() == &entity;
		});
		
		if (pos == manager.m_entities.end())
		{
			m_manager = nullptr;
			return;
		}
		
		m_manager = &manager;
		m_id = pos->m_id;
		m_lastIndex = pos - manager.m_entities.begin();
	}
	*/
	
	Entity* EntityHandleData::GetEntity()
	{
		if (!CheckValidity())
			return nullptr;
		return m_manager->m_entities[m_lastIndex].m_entity.get();
	}
	
	bool EntityHandleData::CheckValidity()
	{
		if (m_manager == nullptr)
			return false;
		
		if (m_lastIndex >= 0 && (size_t)m_lastIndex < m_manager->m_entities.size() &&
			m_manager->m_entities[m_lastIndex].m_entity->GetEntityID() == m_id)
		{
			return true;
		}
		
		m_lastIndex = m_manager->GetEntityIndexById(m_id);
		if (m_lastIndex == -1)
		{
			m_manager = nullptr;
			return false;
		}
		
		return true;
	}
}
