#include "utils.h"

#include <vector>
#include <sstream>
#include <glm/gtc/color_space.hpp>
#include <locale>
#include <codecvt>

namespace TankGame
{
	std::mt19937 randomGen;
	
	std::vector<void(*)()> onCloseCallbacks;
	
	std::stringstream logStream;
	
	const std::string LOG_ERROR = "[error] ";
	const std::string LOG_WARNING = "[warning] ";
	
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
	
	std::u32string UTF8ToUTF32(const std::string & utf8String)
	{
		//MSVC's codecvt is bugged...
		
#ifdef _MSC_VER
		std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> conv;
		auto utf32String = conv.from_bytes(utf8String);
		
		return reinterpret_cast<const char32_t*>(utf32String.c_str());
#else
		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
		return conv.from_bytes(utf8String);
#endif
	}
}
