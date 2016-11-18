#pragma once

#include <cstddef>
#include <glm/glm.hpp>
#include <random>

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

namespace TankGame
{
	extern std::mt19937 randomGen;
	
	template <typename T, size_t I>
	inline constexpr size_t ArrayLength(T (& array)[I])
	{
		return I;
	}
	
	inline bool FloatEqual(float a, float b)
	{
		return std::abs(a - b) < 1E-6f;
	}
	
	std::vector<std::string> Split(const std::string& string, const std::string& delimiters = " ",
	                               bool trimEmpty = false);
	
	void UpdateTransition(float& value, float target, float dt);
	
	void CallOnClose(void(*callback)());
	
	glm::vec3 ParseColorHexCodeSRGB(int hex);
	
	uint32_t RgbColorToSRGBHex(glm::vec3 color);

	std::u32string UTF8ToUTF32(const std::string& utf8String);
	
	extern const std::string LOG_ERROR;
	extern const std::string LOG_WARNING;
	
	std::ostream& GetLogStream();
}
