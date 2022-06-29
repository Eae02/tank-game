#include "bufferallocator.h"
#include "../../utils/utils.h"

#include <iostream>

namespace TankGame
{
	std::unique_ptr<BufferAllocator> BufferAllocator::s_instance;
	
	BufferAllocator::UniquePtr BufferAllocator::AllocateUnique(GLuint size, BufferUsage usage)
	{
		GLuint logBufferSize = std::max<GLuint>(static_cast<GLuint>(std::ceil(std::log2(static_cast<float>(size)))), 4);
		
		/*if (logBufferSize > 7)
		{
			m_buffers.emplace_back(size + (16 - size % 16), usage);
			m_buffers.back().m_isAllocated = true;
			return m_buffers.back().m_buffer.GetID();
		}*/
		
		GLuint bufferSize = 1 << logBufferSize;
		
		auto bufferPos = std::find_if(m_buffers.begin(), m_buffers.end(), [&] (const Buffer& buffer)
		{
			return !buffer.m_isAllocated && buffer.m_size == bufferSize && buffer.m_usage == usage;
		});
		
		if (bufferPos != m_buffers.end())
		{
			bufferPos->m_isAllocated = true;
			return UniquePtr(*this, bufferPos - m_buffers.begin());
		}
		
		m_buffers.emplace_back(bufferSize, usage);
		m_buffers.back().m_isAllocated = true;
		
		return UniquePtr(*this, m_buffers.size() - 1);
	}
	
	void BufferAllocator::Free(size_t index)
	{
		assert(index < m_buffers.size() && m_buffers[index].m_isAllocated);
		m_buffers[index].m_isAllocated = false;
	}
}
