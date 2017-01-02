#include "rendersettingsbuffer.h"
#include "viewinfo.h"
#include "../settings.h"

#include <cstring>

namespace TankGame
{
	RenderSettingsBuffer::RenderSettingsBuffer()
	    : m_uniformBuffer(sizeof(float) * 32, GL_MAP_WRITE_BIT) { }
	
	void RenderSettingsBuffer::Update(const ViewInfo& viewInfo, glm::vec3 eyePosition, float time)
	{
		void* bufferMemory = glMapNamedBuffer(m_uniformBuffer.GetID(), GL_WRITE_ONLY);
		
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
		
		glUnmapNamedBuffer(m_uniformBuffer.GetID());
	}
	
	void RenderSettingsBuffer::OnResize(int width, int height)
	{
		m_resWidth = width;
		m_resHeight = height;
	}
}
