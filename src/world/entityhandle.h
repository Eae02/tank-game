#pragma once

#include "entity.h"

namespace TankGame
{
	class EntityHandleData
	{
	public:
		EntityHandleData() : m_manager(nullptr) { }
		
		EntityHandleData(class EntitiesManager& manager, uint64_t id, long index)
			: m_manager(&manager), m_id(id), m_lastIndex(index) { }
		
		bool CheckValidity();
		
		Entity* GetEntity();
		
		long GetLastIndex() const { return m_lastIndex; }
		
	private:
		class EntitiesManager* m_manager;
		uint64_t m_id;
		long m_lastIndex;
	};
	
	template <typename T = Entity>
	class EntityHandle
	{
		friend class EntitiesManager;
		
	public:
		EntityHandle() = default;
		
		EntityHandle(class EntitiesManager& manager, T& entity)
			: m_data(manager, entity.GetEntityID(), -1), m_entity(&entity) { }
		
		void Despawn() const
		{
			if (Entity* entity = Get())
				entity->Despawn();
		}
		
		bool IsAlive() const
		{ return Get() != nullptr; }
		
		T* operator->() const
		{ return Get(); }
		
		T& operator*() const
		{ return *Get(); }
		
		T* Get() const
		{
			if (!m_data.CheckValidity())
				return nullptr;
			return m_entity;
		}
		
		Entity* GetEntity() const
		{
			return m_data.GetEntity();
		}
		
		template <typename U>
		EntityHandle<U> DynamicCast() const
		{
			if (U* u = dynamic_cast<U*>(Get()))
				return EntityHandle<U>(m_data, u);
			return EntityHandle<U>();
		}
		
	private:
		EntityHandle(EntityHandleData data, T* entity) : m_data(data), m_entity(entity) { }
		
		mutable EntityHandleData m_data;
		T* m_entity;
	};
}
