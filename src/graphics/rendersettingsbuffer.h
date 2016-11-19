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
		
	private:
		Buffer m_uniformBuffer;
	};
}
