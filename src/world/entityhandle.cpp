#include "entityhandle.h"
#include "entitiesmanager.h"

namespace TankGame
{
	Entity*EntityHandle::Get()
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
	
	bool EntityHandle::IsValid()
	{
		if (m_manager == nullptr || m_lastIndex < 0 || m_lastIndex >= static_cast<long>(m_manager->m_entities.size()))
			return false;
		return m_manager->m_entities[m_lastIndex].m_id == m_id;
	}
}
