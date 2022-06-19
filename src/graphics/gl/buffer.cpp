#include "buffer.h"
#include "../frames.h"
#include "../../utils/mathutils.h"

#include <algorithm>
#include <cstring>

namespace TankGame
{
	Buffer::Buffer(size_t size, const void* data, BufferUsage usage) : m_size(size)
	{
		if (usage == BufferUsage::MapWritePersistentMultiFrame)
		{
			m_sizePerFrame = RoundToNextMultiple(size, GetUniformBufferOffsetAlignment());
			m_fullSize = m_sizePerFrame * MAX_QUEUED_FRAMES;
		}
		else
		{
			m_sizePerFrame = 0;
			m_fullSize = size;
		}
		
		GLuint buffer;
		glCreateBuffers(1, &buffer);
		SetID(buffer);
		
		bool persistentMap = usage == BufferUsage::MapWritePersistent || usage == BufferUsage::MapWritePersistentMultiFrame;
		
		if (hasBufferStorage)
		{
			GLbitfield flags = 0;
			if (usage == BufferUsage::DynamicData)
				flags = GL_DYNAMIC_STORAGE_BIT;
			else if (persistentMap)
				flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
			
			glNamedBufferStorage(buffer, m_fullSize, data, flags);
			
			if (persistentMap)
			{
				static constexpr GLbitfield MAP_FLAGS = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT;
				m_mappedMemory = static_cast<char*>(glMapNamedBufferRange(buffer, 0, m_fullSize, MAP_FLAGS));
			}
		}
		else
		{
			GLenum flags = usage == BufferUsage::StaticData ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
			
			glBindBuffer(GL_ARRAY_BUFFER, buffer);
			glBufferData(GL_ARRAY_BUFFER, m_fullSize, data, flags);
			
			if (persistentMap)
			{
				m_fakeMemoryMapping = std::make_unique<char[]>(m_fullSize);
				m_mappedMemory = m_fakeMemoryMapping.get();
			}
		}
		
		if (persistentMap)
			std::memset(m_mappedMemory, 0, m_fullSize);
		else
			m_mappedMemory = nullptr;
	}
	
	size_t Buffer::CurrentFrameOffset() const
	{
		return m_sizePerFrame * GetFrameQueueIndex();
	}
	
	void Buffer::FlushMappedMemory(size_t offset, size_t range)
	{
		if (m_fakeMemoryMapping)
		{
			glNamedBufferSubData(GetID(), offset, range, m_fakeMemoryMapping.get() + offset);
		}
		else
		{
			glFlushMappedNamedBufferRange(GetID(), offset, range);
		}
	}
	
	void Buffer::Update(size_t offset, size_t range, const void* data)
	{
		glNamedBufferSubData(GetID(), offset, range, data);
	}
	
	void DeleteBuffer(GLuint id)
	{
		glDeleteBuffers(1, &id);
	}
	
	static size_t uniformBufferOffsetAlignment = 0;
	
	size_t GetUniformBufferOffsetAlignment()
	{
		if (uniformBufferOffsetAlignment == 0)
		{
			GLint value;
			glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &value);
			uniformBufferOffsetAlignment = static_cast<size_t>(std::max(value, 0));
		}
		
		return uniformBufferOffsetAlignment;
	}
}
