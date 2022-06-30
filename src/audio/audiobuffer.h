#pragma once

#include "alresource.h"

#include <span>

namespace TankGame
{
	class AudioBuffer : public ALResource
	{
	public:
		AudioBuffer();
		
		void SetData(bool isStereo, std::span<const char> data, int frequency);
		
		static AudioBuffer FromOGG(const std::string& path);
		
#ifdef TG_AUDIO_DYNAMIC_LOAD
		void LoadAudioFunctions(void* alLibrary, void* oggVorbisLibrary);
#endif
		
	private:
		static uint32_t CreateAudioBuffer();
	};
}
