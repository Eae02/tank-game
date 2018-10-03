#pragma once

#include "alresource.h"

#include <string>

namespace TankGame
{
	void DestroyAudioBuffer(ALuint buffer);
	
	class AudioBuffer : public ALResource<&DestroyAudioBuffer>
	{
	public:
		inline AudioBuffer()
		    : ALResource(CreateAudioBuffer()) { }
		
		void SetData(ALenum format, void* data, ALsizei dataSize, ALsizei frequency);
		
		static AudioBuffer FromOGG(const std::string& path);
		
	private:
		static ALuint CreateAudioBuffer();
	};
}
