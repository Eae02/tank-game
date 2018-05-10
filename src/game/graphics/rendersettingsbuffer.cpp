#include "rendersettingsbuffer.h"
#include "viewinfo.h"
#include "../settings.h"
#include "../utils/mathutils.h"

#include <cstring>

namespace TankGame
{
	static const size_t BUFFER_SIZE = sizeof(float) * 32;
	
	RenderSettingsBuffer::RenderSettingsBuffer()
	    : m_bufferSize(RoundToNextMultiple(BUFFER_SIZE, GetUniformBufferOffsetAlignment())),
	      m_uniformBuffer(m_bufferSize * MAX_QUEUED_FRAMES, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT),
	      m_ubMemory(reinterpret_cast<char*>(glMapNamedBufferRange(m_uniformBuffer.GetID(), 0,
	                 m_bufferSize * MAX_QUEUED_FRAMES, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT))) { }
	
	void RenderSettingsBuffer::Update(const ViewInfo& viewInfo, glm::vec3 eyePosition, float time)
	{
		char* bufferMemory = m_ubMemory + GetFrameQueueIndex() * m_bufferSize;
		
		memcpy(reinterpret_cast<float*>(bufferMemory) + 0, &viewInfo.GetViewMatrix()[0], sizeof(float) * 3);
		memcpy(reinterpret_cast<float*>(bufferMemory) + 4, &viewInfo.GetViewMatrix()[1], sizeof(float) * 3);
		memcpy(reinterpret_cast<float*>(bufferMemory) + 8, &viewInfo.GetViewMatrix()[2], sizeof(float) * 3);
		
		memcpy(reinterpret_cast<float*>(bufferMemory) + 12, &eyePosition, sizeof(float) * 3);
		reinterpret_cast<float*>(bufferMemory)[15] = time;
		
		memcpy(reinterpret_cast<float*>(bufferMemory) + 16, &viewInfo.GetInverseViewMatrix()[0], sizeof(float) * 3);
		memcpy(reinterpret_cast<float*>(bufferMemory) + 20, &viewInfo.GetInverseViewMatrix()[1], sizeof(float) * 3);
		memcpy(reinterpret_cast<float*>(bufferMemory) + 24, &viewInfo.GetInverseViewMatrix()[2], sizeof(float) * 3);
		
		double resScale = static_cast<double>(Settings::GetInstance().GetResolutionScale());
		
		reinterpret_cast<int32_t*>(bufferMemory)[28] = m_resWidth;
		reinterpret_cast<int32_t*>(bufferMemory)[29] = m_resHeight;
		reinterpret_cast<int32_t*>(bufferMemory)[30] = static_cast<double>(m_resWidth) * resScale;
		reinterpret_cast<int32_t*>(bufferMemory)[31] = static_cast<double>(m_resHeight) * resScale;
		
		glFlushMappedNamedBufferRange(m_uniformBuffer.GetID(), GetFrameQueueIndex() * m_bufferSize, BUFFER_SIZE);
	}
	
	void RenderSettingsBuffer::Bind() const
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uniformBuffer.GetID(),
		                  GetFrameQueueIndex() * m_bufferSize, BUFFER_SIZE);
	}
	
	void RenderSettingsBuffer::OnResize(int width, int height)
	{
		m_resWidth = width;
		m_resHeight = height;
	}
}
