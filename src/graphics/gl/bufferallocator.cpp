#include "bufferallocator.h"
#include "../../utils/utils.h"

#include <algorithm>
#include <iostream>

namespace TankGame
{
	std::unique_ptr<BufferAllocator> BufferAllocator::s_instance;
	
	GLuint BufferAllocator::Allocate(GLuint size, GLbitfield flags)
	{
		GLuint logBufferSize = std::max<GLuint>(static_cast<GLuint>(std::ceil(std::log2(static_cast<float>(size)))), 4);
		
		if (logBufferSize > 7)
		{
			m_buffers.emplace_back(size + (16 - size % 16), flags);
			m_buffers.back().m_isAllocated = true;
			return m_buffers.back().m_buffer.GetID();
		}
		
		GLuint bufferSize = 1 << logBufferSize;
		
		auto bufferPos = std::find_if(m_buffers.begin(), m_buffers.end(), [&] (const Buffer& buffer)
		{
			return !buffer.m_isAllocated && buffer.m_size == bufferSize && (buffer.m_flags & flags) == flags;
		});
		
		if (bufferPos != m_buffers.end())
		{
			bufferPos->m_isAllocated = true;
			return bufferPos->m_buffer.GetID();
		}
		
		m_buffers.emplace_back(bufferSize, flags);
		m_buffers.back().m_isAllocated = true;
		
		return m_buffers.back().m_buffer.GetID();
	}
	
	void BufferAllocator::Free(GLuint buffer)
	{
		auto bufferPos = std::find_if(m_buffers.begin(), m_buffers.end(), [&] (const Buffer& b)
		{ return b.m_buffer.GetID() == buffer; });
		
		if (bufferPos != m_buffers.end())
		{
			if (bufferPos->m_size > (1 << 7))
			{
				*bufferPos = std::move(m_buffers.back());
				m_buffers.pop_back();
			}
			else
				bufferPos->m_isAllocated = false;
		}
	}
	
	BufferAllocator::UniquePtr::~UniquePtr()
	{
		if (m_allocator != nullptr)
			m_allocator->Free(m_buffer);
	}
	
	BufferAllocator::UniquePtr& BufferAllocator::UniquePtr::operator=(BufferAllocator::UniquePtr&& other)
	{
		this->~UniquePtr();
		
		m_allocator = other.m_allocator;
		m_buffer = other.m_buffer;
		
		other.m_allocator = nullptr;
		
		return *this;
	}
}
