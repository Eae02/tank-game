#pragma once

#include <glm/glm.hpp>

#include "gl/buffer.h"
#include "frames.h"

namespace TankGame
{
	class RenderSettingsBuffer
	{
	public:
		RenderSettingsBuffer();
		
		void Update(const class ViewInfo& viewInfo, glm::vec3 eyePosition, float time);
		
		inline void Bind() const
		{
			glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uniformBuffer.GetID(),
			                  GetFrameQueueIndex() * BUFFER_SIZE, BUFFER_SIZE);
		}
		
		void OnResize(int width, int height);
		
	private:
		static constexpr size_t BUFFER_SIZE = sizeof(float) * 32;
		
		Buffer m_uniformBuffer;
		
		char* m_ubMemory;
		
		int m_resWidth = 0, m_resHeight = 0;
	};
}
