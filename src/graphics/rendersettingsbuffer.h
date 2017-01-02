#pragma once

#include <glm/glm.hpp>

#include "gl/buffer.h"

namespace TankGame
{
	class RenderSettingsBuffer
	{
	public:
		RenderSettingsBuffer();
		
		void Update(const class ViewInfo& viewInfo, glm::vec3 eyePosition, float time);
		
		inline void Bind() const
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniformBuffer.GetID());
		}
		
		void OnResize(int width, int height);
		
	private:
		Buffer m_uniformBuffer;
		int m_resWidth = 0, m_resHeight = 0;
	};
}
