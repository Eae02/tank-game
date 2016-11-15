#include "audiobuffer.h"
#include "../utils/ioutils.h"

#include <vorbis/vorbisfile.h>
#include <memory>
#include <cstdio>

namespace TankGame
{
	void DestroyAudioBuffer(ALuint buffer)
	{
		alDeleteBuffers(1, &buffer);
	}
	
	AudioBuffer::AudioBuffer()
	{
		ALuint id;
		alGenBuffers(1, &id);
		SetID(id);
	}
	
	void AudioBuffer::SetData(ALenum format, void* data, ALsizei dataSize, ALsizei frequency)
	{
		alBufferData(GetID(), format, data, dataSize, frequency);
	}
	
	AudioBuffer AudioBuffer::FromOGG(const std::string& path)
	{
		FILE* file = fopen(path.c_str(), "rb");
		if (file == nullptr)
			throw std::runtime_error("Error opening file for reading: '" + path + "'.");
		
		OggVorbis_File oggFile;
		ov_open(file, &oggFile, nullptr, 0);
		
		vorbis_info* info = ov_info(&oggFile, -1);
		
		ALenum format = info->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		
		int bitStream;
		
		std::vector<char> data;
		
		char buffer[512];
		long bytes;
		do
		{
			bytes = ov_read(&oggFile, buffer, sizeof(buffer), 0, 2, 1, &bitStream);
			data.insert(data.end(), buffer, buffer + bytes);
		} while (bytes != 0);
		
		AudioBuffer audioBuffer;
		audioBuffer.SetData(format, data.data(), data.size(), info->rate);
		
		ov_clear(&oggFile);
		
		return audioBuffer;
	}
}
