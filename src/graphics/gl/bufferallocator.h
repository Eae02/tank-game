#pragma once

#include <vector>
#include <memory>

#include "functions.h"
#include "buffer.h"

namespace TankGame
{
	class BufferAllocator
	{
	public:
		class UniquePtr final
		{
		public:
			friend class BufferAllocator;
			
			UniquePtr(decltype(nullptr) = nullptr) { }
			
			UniquePtr(UniquePtr&& other)
			    : m_allocator(other.m_allocator), m_index(other.m_index)
			{
				other.m_allocator = nullptr;
			}
			
			UniquePtr& operator=(UniquePtr&& other)
			{
				this->~UniquePtr();
				m_allocator = other.m_allocator;
				m_index = other.m_index;
				other.m_allocator = nullptr;
				return *this;
			}
			
			UniquePtr(const UniquePtr& other) = delete;
			UniquePtr& operator=(const UniquePtr& other) = delete;
			
			~UniquePtr()
			{
				if (m_allocator != nullptr)
					m_allocator->Free(m_index);
			}
			
			TankGame::Buffer& operator*() const
			{ return m_allocator->m_buffers[m_index].m_buffer; }
			
			TankGame::Buffer* operator->() const
			{ return &m_allocator->m_buffers[m_index].m_buffer; }
			
			bool IsNull() const { return m_allocator == nullptr; }
			
		private:
			inline UniquePtr(BufferAllocator& allocator, size_t index)
			    : m_allocator(&allocator), m_index(index) { }
			
			BufferAllocator* m_allocator = nullptr;
			size_t m_index;
		};
		
		friend class UniquePtr;
		
		UniquePtr AllocateUnique(GLuint size, BufferUsage usage);
		
		static void SetInstance(std::unique_ptr<BufferAllocator>&& instance)
		{ s_instance = std::move(instance); }
		
		static BufferAllocator& GetInstance()
		{ return *s_instance; }
		
	private:
		void Free(size_t index);
		
		static std::unique_ptr<BufferAllocator> s_instance;
		
		struct Buffer
		{
			TankGame::Buffer m_buffer;
			GLuint m_size;
			BufferUsage m_usage;
			bool m_isAllocated;
			
			inline Buffer(GLuint size, BufferUsage usage)
				: m_buffer(size, usage), m_size(size), m_usage(usage) { }
		};
		
		std::vector<Buffer> m_buffers;
	};
}
