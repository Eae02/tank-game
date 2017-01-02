#pragma once

#include <vector>
#include <memory>

namespace TankGame
{
	template <typename T>
	class ObjectPool
	{
	public:
		class Deleter
		{
		public:
			inline Deleter() : m_objectPool(nullptr) { }
			
			explicit inline Deleter(ObjectPool<T>& objectPool)
				: m_objectPool(&objectPool) { }
			
			inline void operator()(T* object)
			{
				if (m_objectPool != nullptr && object != nullptr)
					m_objectPool->Delete(object);
			}
			
		private:
			ObjectPool<T>* m_objectPool;
		};
		
		using UniquePtr = std::unique_ptr<T, Deleter>;
		
		explicit ObjectPool(size_t numObjects)
		    : m_objects(reinterpret_cast<T*>(new char[numObjects * sizeof(T)])),
		      m_objectsAllocated(numObjects, false)
		{
			if (numObjects == 0)
				throw std::runtime_error("An object pool cannot have 0 elements.");
		}
		
		virtual ~ObjectPool()
		{
			if (m_objects == nullptr)
				return;
			
			for (size_t i = 0; i < m_objectsAllocated.size(); i++)
			{
				if (m_objectsAllocated[i])
					m_objects[i].~T();
			}
			
			delete[] reinterpret_cast<char*>(m_objects);
			m_objects = nullptr;
		}
		
		inline ObjectPool(ObjectPool&& other)
		{ operator=(std::move(other)); }
		
		ObjectPool& operator=(ObjectPool&& other)
		{
			this->~ObjectPool();
			m_objects = other.m_objects;
			m_objectsAllocated = std::move(other.m_objectsAllocated);
			m_firstAvailableIndex = other.m_firstAvailableIndex;
			
			other.m_objects = nullptr;
			
			return *this;
		}
		
		template <typename... Args>
		inline UniquePtr MakeUnique(Args&&... args)
		{
			return UniquePtr(New(args...), Deleter(*this));
		}
		
		template <typename... Args>
		T* New(Args&&... args)
		{
			if (m_firstAvailableIndex == -1)
				return nullptr;
			
			T* object = new (m_objects + m_firstAvailableIndex) T(args...);
			m_objectsAllocated[m_firstAvailableIndex] = true;
			
			while (m_firstAvailableIndex != -1 && m_objectsAllocated[m_firstAvailableIndex])
			{
				m_firstAvailableIndex++;
				if (m_firstAvailableIndex >= static_cast<long>(m_objectsAllocated.size()))
					m_firstAvailableIndex = -1;
			}
			
			return object;
		}
		
		void Delete(T* object)
		{
			size_t index = object - m_objects;
			if (index > m_objectsAllocated.size())
				throw std::runtime_error("Attempted to delete an invalid object.");
			
			object->~T();
			
			m_firstAvailableIndex = std::min<long>(index, m_firstAvailableIndex);
			m_objectsAllocated[index] = false;
		}
		
	private:
		T* m_objects;
		std::vector<bool> m_objectsAllocated;
		
		long m_firstAvailableIndex = 0;
	};
}
