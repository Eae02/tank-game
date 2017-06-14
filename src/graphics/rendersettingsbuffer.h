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
		
		void Bind() const;
		
		void OnResize(int width, int height);
		
	private:
		size_t m_bufferSize;
		
		Buffer m_uniformBuffer;
		
		char* m_ubMemory;
		
		int m_resWidth = 0, m_resHeight = 0;
	};
}
