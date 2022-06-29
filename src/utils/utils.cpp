#include "utils.h"
#include "../platform/messagebox.h"

#include <sstream>
#include <iostream>
#include <glm/gtc/color_space.hpp>
#include <locale>
#include <codecvt>

#ifdef __linux__
#include <signal.h>
#endif

namespace TankGame
{
	std::vector<void(*)()> onCloseCallbacks;
	
	std::stringstream logStream;
	
	const std::string LOG_ERROR = "[error] ";
	const std::string LOG_WARNING = "[warning] ";
	const std::string LOG_PERFORMANCE = "[perf] ";
	
	void CallOnClose(void (*callback)())
	{
		onCloseCallbacks.push_back(callback);
	}
	
	void OnClose()
	{
		for (void(*callback)() : onCloseCallbacks)
			callback();
		onCloseCallbacks.clear();
	}
	
	double frameBeginTime;
	
	std::ostream& GetLogStream()
	{
		return logStream;
	}
	
	std::vector<std::string> Split(const std::string& string, const std::string& delimiters, bool trimEmpty)
	{
		std::vector<std::string> tokens;
		
		size_t lastPos = 0;
		
		while (lastPos < string.length() + 1)
		{
			size_t pos = string.find_first_of(delimiters, lastPos);
			if (pos == std::string::npos)
				pos = string.length();
			
			if (pos != lastPos || !trimEmpty)
				tokens.emplace_back(string.data() + lastPos, pos - lastPos);
			
			lastPos = pos + 1;
		}
		
		return tokens;
	}
	
	glm::vec3 ParseColorHexCodeSRGB(int hex)
	{
		glm::vec3 color;
		
		color.r = ((hex & 0xFF0000) >> (8 * 2)) / 255.0f;
		color.g = ((hex & 0x00FF00) >> (8 * 1)) / 255.0f;
		color.b = ((hex & 0x0000FF) >> (8 * 0)) / 255.0f;
		
		return glm::convertSRGBToLinear(color);
	}
	
	uint32_t RgbColorToSRGBHex(glm::vec3 color)
	{
		color = glm::convertLinearToSRGB(color);
		
		uint32_t r = glm::min<uint32_t>(static_cast<uint32_t>(color.r * 256.0f), 255) & 0xFF;
		uint32_t g = glm::min<uint32_t>(static_cast<uint32_t>(color.g * 256.0f), 255) & 0xFF;
		uint32_t b = glm::min<uint32_t>(static_cast<uint32_t>(color.b * 256.0f), 255) & 0xFF;
		
		return (r << 16) | (g << 8) | b;
	}
	
	void UpdateTransition(float& value, float target, float dt)
	{
		float delta = target - value;
		value += glm::min(dt, glm::abs(delta)) * glm::sign(delta);
	}
	
	[[noreturn]] static inline void PanicBreakOrErrorMessage(const std::string& message)
	{
#if !defined(NDEBUG) && defined(__linux__)
		raise(SIGTRAP);
#else
		ShowErrorMessage(message, "Unexpected Error");
#endif
		std::exit(1);
	}
	
#ifdef __cpp_lib_source_location
	void Panic(const std::string& message, std::source_location location)
	{
		std::cerr <<
			"Panic:\n"
			"  " << message << "\n"
			"  " << location.function_name() << "\n" 
			"  " << location.file_name() << ":" << location.line() << std::endl;
		PanicBreakOrErrorMessage(message);
	}
#else
	void Panic(const std::string& message)
	{
		std::cerr << "Panic:\n  " << message << std::endl;
		PanicBreakOrErrorMessage(message);
	}
#endif
}
