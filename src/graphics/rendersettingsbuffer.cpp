#include "rendersettingsbuffer.h"
#include "viewinfo.h"
#include "../settings.h"
#include "../utils/mathutils.h"

#include <cstring>

namespace TankGame
{
	static const size_t BUFFER_SIZE = sizeof(float) * 32;
	
	RenderSettingsBuffer::RenderSettingsBuffer()
	    : m_uniformBuffer(BUFFER_SIZE, BufferUsage::MapWritePersistentUBO_MultiFrame) { }
	
	void RenderSettingsBuffer::Update(const ViewInfo& viewInfo, glm::vec3 eyePosition, float time)
	{
		char* bufferMemory = m_uniformBuffer.CurrentFrameMappedMemory();
		
		memcpy(reinterpret_cast<float*>(bufferMemory) + 0, &viewInfo.GetViewMatrix()[0], sizeof(float) * 3);
		memcpy(reinterpret_cast<float*>(bufferMemory) + 4, &viewInfo.GetViewMatrix()[1], sizeof(float) * 3);
		memcpy(reinterpret_cast<float*>(bufferMemory) + 8, &viewInfo.GetViewMatrix()[2], sizeof(float) * 3);
		
		memcpy(reinterpret_cast<float*>(bufferMemory) + 12, &eyePosition, sizeof(float) * 3);
		reinterpret_cast<float*>(bufferMemory)[15] = time;
		
		memcpy(reinterpret_cast<float*>(bufferMemory) + 16, &viewInfo.GetInverseViewMatrix()[0], sizeof(float) * 3);
		memcpy(reinterpret_cast<float*>(bufferMemory) + 20, &viewInfo.GetInverseViewMatrix()[1], sizeof(float) * 3);
		memcpy(reinterpret_cast<float*>(bufferMemory) + 24, &viewInfo.GetInverseViewMatrix()[2], sizeof(float) * 3);
		
		double resScale = static_cast<double>(Settings::instance.GetResolutionScale());
		
		reinterpret_cast<int32_t*>(bufferMemory)[28] = m_resWidth;
		reinterpret_cast<int32_t*>(bufferMemory)[29] = m_resHeight;
		reinterpret_cast<int32_t*>(bufferMemory)[30] = static_cast<double>(m_resWidth) * resScale;
		reinterpret_cast<int32_t*>(bufferMemory)[31] = static_cast<double>(m_resHeight) * resScale;
		
		m_uniformBuffer.FlushCurrentFrameMappedMemory();
	}
	
	void RenderSettingsBuffer::Bind() const
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uniformBuffer.GetID(), m_uniformBuffer.CurrentFrameOffset(), BUFFER_SIZE);
	}
	
	void RenderSettingsBuffer::OnResize(int width, int height)
	{
		m_resWidth = width;
		m_resHeight = height;
	}
}
