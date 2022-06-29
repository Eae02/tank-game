#pragma once


#include "entity.h"

namespace TankGame
{
	class EntityHandle
	{
		friend class EntitiesManager;
		
	public:
		inline EntityHandle() : m_manager(nullptr) { }
		
		EntityHandle(class EntitiesManager& manager, const class Entity& entity);
		
		inline void Despawn() const
		{
			if (Entity* entity = Get())
				entity->Despawn();
		}
		
		inline bool IsNull() const
		{ return m_manager == nullptr; }
		
		inline bool IsAlive() const
		{ return Get() != nullptr; }
		
		inline class Entity* operator->() const
		{ return Get(); }
		
		inline class Entity& operator*() const
		{ return *Get(); }
		
		class Entity* Get() const;
		
	private:
		EntityHandle(class EntitiesManager& manager, uint64_t id, long index = -1);
		
		void UpdateLastIndex() const;
		
		bool IsValid() const;
		
		class EntitiesManager* m_manager;
		uint64_t m_id;
		mutable long m_lastIndex;
	};
}
