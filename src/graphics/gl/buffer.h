#pragma once

#include "glresource.h"

#include <memory>

namespace TankGame
{
	size_t GetUniformBufferOffsetAlignment();
	
	void DeleteBuffer(GLuint id);
	
	enum class BufferUsage
	{
		StaticData,
		DynamicData,
		MapWritePersistent,
		MapWritePersistentMultiFrame
	};
	
	class Buffer : public GLResource<DeleteBuffer>
	{
	public:
		inline Buffer(size_t size, BufferUsage usage)
		    : Buffer(size, nullptr, usage) { }
		
		Buffer(size_t size, const void* data, BufferUsage usage);
		
		char* MappedMemory() const { return m_mappedMemory; }
		void FlushMappedMemory(size_t offset, size_t range);
		
		void Update(size_t offset, size_t range, const void* data);
		
		size_t CurrentFrameOffset() const;
		char* CurrentFrameMappedMemory() const { return m_mappedMemory + CurrentFrameOffset(); }
		void FlushCurrentFrameMappedMemory() { FlushMappedMemory(CurrentFrameOffset(), m_size); }
		
		size_t Size() const { return m_size; }
		size_t SizePerFrame() const { return m_size; }
		size_t FullSize() const { return m_size; }
		
	private:
		char* m_mappedMemory;
		std::unique_ptr<char[]> m_fakeMemoryMapping;
		size_t m_size;
		size_t m_sizePerFrame;
		size_t m_fullSize;
	};
}
