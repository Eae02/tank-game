#ifdef TG_AUDIO_DYNAMIC_LOAD
#include "audiolib.h"
#include "../platform/common.h"
#include "../utils/utils.h"

namespace TankGame
{
#define AL_FUNC(name, lpname) lpname name;
#include "alfunctionslist.inl"
#undef AL_FUNC
	
	ov_open_func ov_open;
	ov_info_func ov_info;
	ov_read_func ov_read;
	ov_clear_func ov_clear;
	
#if defined(__linux__)
	static const char* OPENAL_LIB_NAME = "libopenal.so";
	static const char* VORBISFILE_LIB_NAME = "libvorbisfile.so";
#elif defined(_WIN32)
	static const char* OPENAL_LIB_NAME = "OpenAL32.dll";
	static const char* VORBISFILE_LIB_NAME = "libvorbisfile-3.dll";
#else
#error "TG_AUDIO_DYNAMIC_LOAD is defined but audio library names are not set for this platform"
#endif
	
	static inline void* TryLoadDynamicLibrary(const char* name)
	{
		void* lib = DLOpen(name);
		if (lib == nullptr)
			GetLogStream() << LOG_ERROR << "Failed to load dynamic library " << name << "\n";
		return lib;
	}
	
	template <typename T>
	static inline bool TryLoadLibrarySymbol(void* library, const char* symbolName, T& symbolOut)
	{
		void* symbol = DLSym(library, symbolName);
		if (symbol == nullptr)
		{
			GetLogStream() << LOG_ERROR << "Audio function failed to load: " << symbolName << "\n";
			return false;
		}
		symbolOut = reinterpret_cast<T>(symbol);
		return true;
	}
	
	bool LoadAudioFunctions()
	{
		void* openalLib = TryLoadDynamicLibrary(OPENAL_LIB_NAME);
		if (openalLib == nullptr) return false;
		
		void* vorbisfileLib = TryLoadDynamicLibrary(VORBISFILE_LIB_NAME);
		if (vorbisfileLib == nullptr) return false;
		
#define AL_FUNC(name, _) if (!TryLoadLibrarySymbol(openalLib, #name, TankGame::name)) return false;
#include "alfunctionslist.inl"
#undef AL_FUNC
		
		if (!TryLoadLibrarySymbol(vorbisfileLib, "ov_open", TankGame::ov_open)) return false;
		if (!TryLoadLibrarySymbol(vorbisfileLib, "ov_info", TankGame::ov_info)) return false;
		if (!TryLoadLibrarySymbol(vorbisfileLib, "ov_read", TankGame::ov_read)) return false;
		if (!TryLoadLibrarySymbol(vorbisfileLib, "ov_clear", TankGame::ov_clear)) return false;
		
		return true;
	}
	
	void InstallNoOpAudioFunctions()
	{
#define AL_FUNC(name, lpname) \
		TankGame::name = [] (auto... args) { \
			using RetType = std::invoke_result_t<lpname, decltype(args)...>; \
			if constexpr (!std::is_void_v<RetType>) return RetType { }; \
		};
#include "alfunctionslist.inl"
#undef AL_FUNC
	}
}

#endif
