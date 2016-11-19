#include "rendersettingsbuffer.h"
#include "viewinfo.h"

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
		
		glUnmapNamedBuffer(m_uniformBuffer.GetID());
	}
}
