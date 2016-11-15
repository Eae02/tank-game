#pragma once

#include <type_traits>
#include <algorithm>

namespace TankGame
{
	template <typename T>
	class StackObject final
	{
	public:
		inline StackObject() : m_isNull(true) { }
		
		inline ~StackObject()
		{
			Destroy();
		}
		
		StackObject(const StackObject<T>& other) = delete;
		StackObject<T>& operator=(const StackObject<T>& other) = delete;
		
		StackObject(StackObject<T>&& other) : m_isNull(other.m_isNull)
		{
			std::copy(std::begin(other.m_memory), std::end(other.m_memory), m_memory);
			other.m_isNull = true;
		}
		
		StackObject<T>& operator=(StackObject<T>&& other)
		{
			Destroy();
			
			if (!other.m_isNull)
			{
				std::copy(std::begin(other.m_memory), std::end(other.m_memory), m_memory);
				
				m_isNull = false;
				other.m_isNull = true;
			}
			
			return *this;
		}
		
		template <typename... Args>
		inline void Construct(Args&&... args)
		{
			Destroy();
			new (m_memory) T(std::forward<Args>(args)...);
			m_isNull = false;
		}
		
		void Destroy()
		{
			if (!m_isNull)
			{
				reinterpret_cast<T*>(m_memory)->~T();
				m_isNull = true;
			}
		}
		
		inline bool IsNull() const
		{ return m_isNull; }
		
		inline T* Get()
		{ return m_isNull ? nullptr : reinterpret_cast<T*>(m_memory); }
		inline const T* Get() const
		{ return m_isNull ? nullptr : reinterpret_cast<const T*>(m_memory); }
		
		inline T& operator*()
		{ return *Get(); }
		inline const T& operator*() const
		{ return *Get(); }
		
		inline T* operator->()
		{ return Get(); }
		inline const T* operator->() const
		{ return Get(); }
		
	private:
		char m_memory[sizeof(T)];
		bool m_isNull;
	};
}
