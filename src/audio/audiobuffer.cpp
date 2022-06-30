#include "audiobuffer.h"
#include "audiolib.h"
#include "../utils/ioutils.h"
#include "../utils/utils.h"

#include <vorbis/vorbisfile.h>
#include <cstdio>

namespace TankGame
{
	uint32_t AudioBuffer::CreateAudioBuffer()
	{
		uint32_t id;
		alGenBuffers(1, &id);
		return id;
	}
	
	AudioBuffer::AudioBuffer()
		: ALResource(CreateAudioBuffer(), alDeleteBuffers) { }
	
	void AudioBuffer::SetData(bool isStereo, std::span<const char> data, int frequency)
	{
		ALenum format = isStereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
		alBufferData(GetID(), format, data.data(), data.size_bytes(), frequency);
	}
	
	AudioBuffer AudioBuffer::FromOGG(const std::string& path)
	{
		if (!ov_open)
			return AudioBuffer();
		
		FILE* file = fopen(path.c_str(), "rb");
		if (file == nullptr)
			Panic("Error opening audio file for reading: '" + path + "'.");
		
		OggVorbis_File oggFile;
		ov_open(file, &oggFile, nullptr, 0);
		
		vorbis_info* info = ov_info(&oggFile, -1);
		
		std::vector<char> data;
		char buffer[512];
		long bytesRead;
		do
		{
			int bitStream;
			bytesRead = ov_read(&oggFile, buffer, sizeof(buffer), 0, 2, 1, &bitStream);
			data.insert(data.end(), buffer, buffer + bytesRead);
		} while (bytesRead != 0);
		
		AudioBuffer audioBuffer;
		audioBuffer.SetData(info->channels == 2, data, info->rate);
		
		ov_clear(&oggFile);
		
		return audioBuffer;
	}
}
