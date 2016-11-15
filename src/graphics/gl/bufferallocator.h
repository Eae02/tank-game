#pragma once

#include <GL/glew.h>
#include <vector>
#include <memory>

#include "buffer.h"

namespace TankGame
{
	class BufferAllocator
	{
	public:
		class UniquePtr final
		{
		public:
			inline UniquePtr(decltype(nullptr) = nullptr) { }
			
			inline UniquePtr(BufferAllocator& allocator, GLuint buffer)
			    : m_allocator(&allocator), m_buffer(buffer) { }
			
			inline UniquePtr(UniquePtr&& other)
			    : m_allocator(other.m_allocator), m_buffer(other.m_buffer)
			{
				other.m_allocator = nullptr;
			}
			
			UniquePtr& operator=(UniquePtr&& other);
			
			~UniquePtr();
			
			inline GLuint operator*() const
			{ return m_buffer; }
			
		private:
			BufferAllocator* m_allocator = nullptr;
			GLuint m_buffer;
		};
		
		inline UniquePtr AllocateUnique(GLuint size, GLbitfield flags)
		{
			return UniquePtr(*this, Allocate(size, flags));
		}
		
		GLuint Allocate(GLuint size, GLbitfield flags);
		void Free(GLuint buffer);
		
		static void SetInstance(std::unique_ptr<BufferAllocator>&& instance)
		{ s_instance = std::move(instance); }
		
		static BufferAllocator& GetInstance()
		{ return *s_instance; }
		
	private:
		static std::unique_ptr<BufferAllocator> s_instance;
		
		struct Buffer
		{
			TankGame::Buffer m_buffer;
			GLuint m_size;
			GLbitfield m_flags;
			bool m_isAllocated;
			
			inline Buffer(GLuint size, GLbitfield flags)
				: m_buffer(size, flags), m_size(size), m_flags(flags) { }
		};
		
		std::vector<Buffer> m_buffers;
	};
}
