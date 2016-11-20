#pragma once

#include <cstdint>

namespace TankGame
{
	class EntityHandle
	{
		friend class EntitiesManager;
		
	public:
		inline EntityHandle() : m_manager(nullptr) { }
		
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
		
		bool IsValid() const;
		
		uint64_t m_id;
		class EntitiesManager* m_manager;
		
		mutable long m_lastIndex;
	};
}
