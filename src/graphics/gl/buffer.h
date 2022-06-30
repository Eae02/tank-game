#pragma once

#include "glresource.h"

namespace TankGame
{
	size_t GetUniformBufferOffsetAlignment();
	
	void DeleteBuffer(GLuint id);
	
	static constexpr uint32_t BUFFER_USAGE_STATIC_BIT    = 1 << 8;
	static constexpr uint32_t BUFFER_USAGE_MAP_WRITE_BIT = 1 << 9;
	
	enum class BufferUsage : uint32_t
	{
		StaticVertex                     = 0 | BUFFER_USAGE_STATIC_BIT,
		StaticIndex                      = 1 | BUFFER_USAGE_STATIC_BIT,
		StaticUBO                        = 2 | BUFFER_USAGE_STATIC_BIT,
		DynamicUBO                       = 3,
		MapWritePersistentUBO            = 4 | BUFFER_USAGE_MAP_WRITE_BIT,
		MapWritePersistentUBO_MultiFrame = 5 | BUFFER_USAGE_MAP_WRITE_BIT
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
		GLenum m_target;
		size_t m_size;
		size_t m_sizePerFrame;
		size_t m_fullSize;
	};
}
