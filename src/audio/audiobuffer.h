#pragma once

#include "alresource.h"

#include <string>

namespace TankGame
{
	void DestroyAudioBuffer(ALuint buffer);
	
	class AudioBuffer : public ALResource<&DestroyAudioBuffer>
	{
	public:
		AudioBuffer();
		
		void SetData(ALenum format, void* data, ALsizei dataSize, ALsizei frequency);
		
		static AudioBuffer FromOGG(const std::string& path);
	};
}
