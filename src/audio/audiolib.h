#pragma once

#include <AL/al.h>
#include <AL/alc.h>

struct OggVorbis_File;
struct vorbis_info;

namespace TankGame
{
#ifdef TG_AUDIO_DYNAMIC_LOAD
#define AL_FUNC(name, lpname) extern lpname name;
#include "alfunctionslist.inl"
#undef AL_FUNC
	
	using ov_open_func = int(*)(FILE* f, OggVorbis_File* vf, const char* initial, long ibytes);
	using ov_info_func = vorbis_info*(*)(OggVorbis_File* vf, int link);
	using ov_read_func = long(*)(OggVorbis_File* vf, char* buffer, int length, int bigendianp, int word, int sgned, int* bitstream);
	using ov_clear_func = int(*)(OggVorbis_File* vf);
	
	extern ov_open_func ov_open;
	extern ov_info_func ov_info;
	extern ov_read_func ov_read;
	extern ov_clear_func ov_clear;
	
	bool LoadAudioFunctions();
	void InstallNoOpAudioFunctions();
#else
	inline bool LoadAudioFunctions() { return true; }
	inline void InstallNoOpAudioFunctions() { }
#endif
}
