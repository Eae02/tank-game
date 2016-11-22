#include "entityhandle.h"
#include "entitiesmanager.h"

namespace TankGame
{
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
	
	Entity* EntityHandle::Get() const
	{
		if (m_manager == nullptr)
			return nullptr;
		
		if (!IsValid())
		{
			m_lastIndex = m_manager->GetEntityIndexById(m_id);
			if (m_lastIndex == -1)
				return nullptr;
		}
		
		return m_manager->m_entities[m_lastIndex].m_entity.get();
	}
	
	EntityHandle::EntityHandle(EntitiesManager& manager, uint64_t id, long index)
	    : m_manager(&manager), m_id(id), m_lastIndex(index) { }
	
	bool EntityHandle::IsValid() const
	{
		if (m_manager == nullptr || m_lastIndex < 0 || m_lastIndex >= static_cast<long>(m_manager->m_entities.size()))
			return false;
		return m_manager->m_entities[m_lastIndex].m_id == m_id;
	}
}
