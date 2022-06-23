#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <iosfwd>
#include <glm/glm.hpp>

#include <version>
#ifdef __cpp_lib_source_location
#include <source_location>
#endif

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

namespace TankGame
{
	template <typename T, size_t I>
	inline constexpr size_t ArrayLength(T (& array)[I])
	{
		return I;
	}
	
	inline bool FloatEqual(float a, float b)
	{
		return std::abs(a - b) < 1E-6f;
	}
	
	struct FreeDeleter
	{
		void operator()(void* data) const { std::free(data); }
	};
	
	struct IVec2Hash
	{
		size_t operator()(const glm::ivec2& v) const
		{
			if constexpr (sizeof(size_t) >= sizeof(int) * 2)
				return ((size_t)v.x << (size_t)32) | (size_t)v.y;
			else
				return (size_t)v.x + 0x9e3779b9 + ((size_t)v.y << 6) + ((size_t)v.y >> 2);
		}
	};
	
	extern double frameBeginTime;
	
	std::vector<std::string> Split(const std::string& string, const std::string& delimiters = " ",
	                               bool trimEmpty = false);
	
	void UpdateTransition(float& value, float target, float dt);
	
	void CallOnClose(void(*callback)());
	
	glm::vec3 ParseColorHexCodeSRGB(int hex);
	
	uint32_t RgbColorToSRGBHex(glm::vec3 color);

	extern const std::string LOG_ERROR;
	extern const std::string LOG_WARNING;
	extern const std::string LOG_PERFORMANCE;
	
	std::ostream& GetLogStream();
	
#ifdef __cpp_lib_source_location
	[[noreturn]] void Panic(const std::string& message, std::source_location location = std::source_location::current());
#else
	[[noreturn]] void Panic(const std::string& message);
#endif
}
