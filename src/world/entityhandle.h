#pragma once

#include <cstdint>

namespace TankGame
{
	class EntityHandle
	{
		friend class EntitiesManager;
		
	public:
		inline EntityHandle() : m_manager(nullptr) { }
		
		inline bool IsAlive()
		{ return Get() != nullptr; }
		
		inline class Entity* operator->()
		{ return Get(); }
		
		inline class Entity& operator*()
		{ return *Get(); }
		
		class Entity* Get();
		
	private:
		EntityHandle(class EntitiesManager& manager, uint64_t id, long index = -1);
		
		bool IsValid();
		
		uint64_t m_id;
		long m_lastIndex;
		class EntitiesManager* m_manager;
	};
}
