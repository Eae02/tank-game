#include "window.h"
#include "settings.h"
#include "utils/utils.h"
#include "utils/ioutils.h"

#include <json.hpp>
#include <fstream>

namespace TankGame
{
	std::vector<glm::ivec2> Settings::s_resolutions;
	int Settings::s_defaultResolutionIndex = -1;
	
	Settings Settings::s_instance;
	
	Settings::Settings()
	{
		if (s_defaultResolutionIndex != -1)
			m_resolution = s_resolutions[s_defaultResolutionIndex];
	}
	
	Settings::Settings(const fs::path& jsonPath)
	{
		nlohmann::json settingsDocument = nlohmann::json::parse(ReadFileContents(jsonPath));
		
		const nlohmann::json& buttonsEl = settingsDocument["buttons"];
		const nlohmann::json& videoEl = settingsDocument["video"];
		
		MaybeParseButtonFromJSON(buttonsEl, "forward", m_forwardButton);
		MaybeParseButtonFromJSON(buttonsEl, "back", m_backButton);
		MaybeParseButtonFromJSON(buttonsEl, "left", m_leftButton);
		MaybeParseButtonFromJSON(buttonsEl, "right", m_rightButton);
		MaybeParseButtonFromJSON(buttonsEl, "interact", m_interactButton);
		MaybeParseButtonFromJSON(buttonsEl, "fire", m_fireButton);
		
		const nlohmann::json& resolutionEl = videoEl["resolution"];
		m_resolution = { resolutionEl[0].get<int>(), resolutionEl[1].get<int>() };
		
		m_isFullscreen = videoEl["fullscreen"].get<bool>();
		
		if (m_isFullscreen && !s_resolutions.empty())
		{
			if (std::find(s_resolutions.begin(), s_resolutions.end(), m_resolution) == s_resolutions.end())
				m_resolution = s_resolutions[s_defaultResolutionIndex];
		}
		
		m_enableVSync = videoEl["vsync"].get<bool>();
		
		const nlohmann::json& qualityEl = videoEl["quality"];
		
		m_lightingQuality = ParseQualityString(qualityEl["lighting"].get<std::string>(), m_lightingQuality);
		m_particlesQuality = ParseQualityString(qualityEl["particles"].get<std::string>(), m_particlesQuality);
		m_postProcessingQuality = ParseQualityString(qualityEl["post"].get<std::string>(), m_postProcessingQuality);
		
		m_enableBloom = qualityEl["bloom"].get<bool>();
	}
	
	void Settings::Save(const fs::path& jsonPath) const
	{
		nlohmann::json json;
		
		nlohmann::json videoEl;
		videoEl["fullscreen"] = m_isFullscreen;
		videoEl["resolution"] = { m_resolution.x, m_resolution.y };
		videoEl["vsync"] = m_enableVSync;
		
		nlohmann::json qualityEl;
		qualityEl["lighting"] = GetQualityString(m_lightingQuality);
		qualityEl["particles"] = GetQualityString(m_particlesQuality);
		qualityEl["post"] = GetQualityString(m_postProcessingQuality);
		qualityEl["bloom"] = m_enableBloom;
		videoEl["quality"] = qualityEl;
		
		json["video"] = videoEl;
		
		nlohmann::json buttonsEl;
		buttonsEl["forward"] = GetButtonString(m_forwardButton);
		buttonsEl["back"] = GetButtonString(m_backButton);
		buttonsEl["left"] = GetButtonString(m_leftButton);
		buttonsEl["right"] = GetButtonString(m_rightButton);
		buttonsEl["interact"] = GetButtonString(m_interactButton);
		buttonsEl["fire"] = GetButtonString(m_fireButton);
		json["buttons"] = buttonsEl;
		
		std::ofstream stream(jsonPath);
		stream << std::setw(4) << json;
	}
	
	void Settings::DetectVideoModes()
	{
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		
		int videoModeCount;
		const GLFWvidmode* videoModes = glfwGetVideoModes(primaryMonitor, &videoModeCount);
		
		for (int i = 0; i < videoModeCount; i++)
		{
			if (videoModes[i].width < Window::MIN_WIDTH || videoModes[i].height < Window::MIN_HEIGHT)
				continue;
			
			auto pos = std::find_if(s_resolutions.begin(), s_resolutions.end(), [&] (glm::ivec2 res)
			{
				return res.x == videoModes[i].width && res.y == videoModes[i].height;
			});
			
			if (pos == s_resolutions.end())
				s_resolutions.emplace_back(videoModes[i].width, videoModes[i].height);
		}
		
		const GLFWvidmode* defaultVideoMode = glfwGetVideoMode(primaryMonitor);
		s_defaultResolutionIndex = std::find_if(s_resolutions.begin(), s_resolutions.end(), [&] (glm::ivec2 res)
		{
			return res.x == defaultVideoMode->width && res.y == defaultVideoMode->height;
	}) - s_resolutions.begin();
	}
	
	void Settings::MaybeParseButtonFromJSON(const nlohmann::json& json, const std::string& elementName, int& out)
	{
		auto elementIt = json.find(elementName);
		
		if (elementIt != json.end())
			out = ParseButtonString(*elementIt);
	}
	
	int Settings::ParseButtonString(const std::string& string)
	{
		if (strcasecmp(string.c_str(), "mouse1") == 0) return GLFW_MOUSE_BUTTON_1;
		if (strcasecmp(string.c_str(), "mouse2") == 0) return GLFW_MOUSE_BUTTON_2;
		if (strcasecmp(string.c_str(), "mouse3") == 0) return GLFW_MOUSE_BUTTON_3;
		if (strcasecmp(string.c_str(), "mouse4") == 0) return GLFW_MOUSE_BUTTON_4;
		if (strcasecmp(string.c_str(), "mouse5") == 0) return GLFW_MOUSE_BUTTON_5;
		if (strcasecmp(string.c_str(), "mouse6") == 0) return GLFW_MOUSE_BUTTON_6;
		if (strcasecmp(string.c_str(), "mouse7") == 0) return GLFW_MOUSE_BUTTON_7;
		if (strcasecmp(string.c_str(), "mouse8") == 0) return GLFW_MOUSE_BUTTON_8;
		
		if (strcasecmp(string.c_str(), "space") == 0) return GLFW_KEY_SPACE;
		if (strcasecmp(string.c_str(), "apostrophe") == 0) return GLFW_KEY_APOSTROPHE;
		if (strcasecmp(string.c_str(), "comma") == 0) return GLFW_KEY_COMMA;
		if (strcasecmp(string.c_str(), "minus") == 0) return GLFW_KEY_MINUS;
		if (strcasecmp(string.c_str(), "period") == 0) return GLFW_KEY_PERIOD;
		if (strcasecmp(string.c_str(), "slash") == 0) return GLFW_KEY_SLASH;
		if (strcasecmp(string.c_str(), "0") == 0) return GLFW_KEY_0;
		if (strcasecmp(string.c_str(), "1") == 0) return GLFW_KEY_1;
		if (strcasecmp(string.c_str(), "2") == 0) return GLFW_KEY_2;
		if (strcasecmp(string.c_str(), "3") == 0) return GLFW_KEY_3;
		if (strcasecmp(string.c_str(), "4") == 0) return GLFW_KEY_4;
		if (strcasecmp(string.c_str(), "5") == 0) return GLFW_KEY_5;
		if (strcasecmp(string.c_str(), "6") == 0) return GLFW_KEY_6;
		if (strcasecmp(string.c_str(), "7") == 0) return GLFW_KEY_7;
		if (strcasecmp(string.c_str(), "8") == 0) return GLFW_KEY_8;
		if (strcasecmp(string.c_str(), "9") == 0) return GLFW_KEY_9;
		if (strcasecmp(string.c_str(), "semicolon") == 0) return GLFW_KEY_SEMICOLON;
		if (strcasecmp(string.c_str(), "equal") == 0) return GLFW_KEY_EQUAL;
		if (strcasecmp(string.c_str(), "a") == 0) return GLFW_KEY_A;
		if (strcasecmp(string.c_str(), "b") == 0) return GLFW_KEY_B;
		if (strcasecmp(string.c_str(), "c") == 0) return GLFW_KEY_C;
		if (strcasecmp(string.c_str(), "d") == 0) return GLFW_KEY_D;
		if (strcasecmp(string.c_str(), "e") == 0) return GLFW_KEY_E;
		if (strcasecmp(string.c_str(), "f") == 0) return GLFW_KEY_F;
		if (strcasecmp(string.c_str(), "g") == 0) return GLFW_KEY_G;
		if (strcasecmp(string.c_str(), "h") == 0) return GLFW_KEY_H;
		if (strcasecmp(string.c_str(), "i") == 0) return GLFW_KEY_I;
		if (strcasecmp(string.c_str(), "j") == 0) return GLFW_KEY_J;
		if (strcasecmp(string.c_str(), "k") == 0) return GLFW_KEY_K;
		if (strcasecmp(string.c_str(), "l") == 0) return GLFW_KEY_L;
		if (strcasecmp(string.c_str(), "m") == 0) return GLFW_KEY_M;
		if (strcasecmp(string.c_str(), "n") == 0) return GLFW_KEY_N;
		if (strcasecmp(string.c_str(), "o") == 0) return GLFW_KEY_O;
		if (strcasecmp(string.c_str(), "p") == 0) return GLFW_KEY_P;
		if (strcasecmp(string.c_str(), "q") == 0) return GLFW_KEY_Q;
		if (strcasecmp(string.c_str(), "r") == 0) return GLFW_KEY_R;
		if (strcasecmp(string.c_str(), "s") == 0) return GLFW_KEY_S;
		if (strcasecmp(string.c_str(), "t") == 0) return GLFW_KEY_T;
		if (strcasecmp(string.c_str(), "u") == 0) return GLFW_KEY_U;
		if (strcasecmp(string.c_str(), "v") == 0) return GLFW_KEY_V;
		if (strcasecmp(string.c_str(), "w") == 0) return GLFW_KEY_W;
		if (strcasecmp(string.c_str(), "x") == 0) return GLFW_KEY_X;
		if (strcasecmp(string.c_str(), "y") == 0) return GLFW_KEY_Y;
		if (strcasecmp(string.c_str(), "z") == 0) return GLFW_KEY_Z;
		if (strcasecmp(string.c_str(), "left_bracket") == 0) return GLFW_KEY_LEFT_BRACKET;
		if (strcasecmp(string.c_str(), "backslash") == 0) return GLFW_KEY_BACKSLASH;
		if (strcasecmp(string.c_str(), "right_bracket") == 0) return GLFW_KEY_RIGHT_BRACKET;
		if (strcasecmp(string.c_str(), "grave_accent") == 0) return GLFW_KEY_GRAVE_ACCENT;
		if (strcasecmp(string.c_str(), "world_1") == 0) return GLFW_KEY_WORLD_1;
		if (strcasecmp(string.c_str(), "world_2") == 0) return GLFW_KEY_WORLD_2;
		if (strcasecmp(string.c_str(), "escape") == 0) return GLFW_KEY_ESCAPE;
		if (strcasecmp(string.c_str(), "enter") == 0) return GLFW_KEY_ENTER;
		if (strcasecmp(string.c_str(), "tab") == 0) return GLFW_KEY_TAB;
		if (strcasecmp(string.c_str(), "backspace") == 0) return GLFW_KEY_BACKSPACE;
		if (strcasecmp(string.c_str(), "insert") == 0) return GLFW_KEY_INSERT;
		if (strcasecmp(string.c_str(), "delete") == 0) return GLFW_KEY_DELETE;
		if (strcasecmp(string.c_str(), "right") == 0) return GLFW_KEY_RIGHT;
		if (strcasecmp(string.c_str(), "left") == 0) return GLFW_KEY_LEFT;
		if (strcasecmp(string.c_str(), "down") == 0) return GLFW_KEY_DOWN;
		if (strcasecmp(string.c_str(), "up") == 0) return GLFW_KEY_UP;
		if (strcasecmp(string.c_str(), "page_up") == 0) return GLFW_KEY_PAGE_UP;
		if (strcasecmp(string.c_str(), "page_down") == 0) return GLFW_KEY_PAGE_DOWN;
		if (strcasecmp(string.c_str(), "home") == 0) return GLFW_KEY_HOME;
		if (strcasecmp(string.c_str(), "end") == 0) return GLFW_KEY_END;
		if (strcasecmp(string.c_str(), "caps_lock") == 0) return GLFW_KEY_CAPS_LOCK;
		if (strcasecmp(string.c_str(), "scroll_lock") == 0) return GLFW_KEY_SCROLL_LOCK;
		if (strcasecmp(string.c_str(), "num_lock") == 0) return GLFW_KEY_NUM_LOCK;
		if (strcasecmp(string.c_str(), "print_screen") == 0) return GLFW_KEY_PRINT_SCREEN;
		if (strcasecmp(string.c_str(), "pause") == 0) return GLFW_KEY_PAUSE;
		if (strcasecmp(string.c_str(), "f1") == 0) return GLFW_KEY_F1;
		if (strcasecmp(string.c_str(), "f2") == 0) return GLFW_KEY_F2;
		if (strcasecmp(string.c_str(), "f3") == 0) return GLFW_KEY_F3;
		if (strcasecmp(string.c_str(), "f4") == 0) return GLFW_KEY_F4;
		if (strcasecmp(string.c_str(), "f5") == 0) return GLFW_KEY_F5;
		if (strcasecmp(string.c_str(), "f6") == 0) return GLFW_KEY_F6;
		if (strcasecmp(string.c_str(), "f7") == 0) return GLFW_KEY_F7;
		if (strcasecmp(string.c_str(), "f8") == 0) return GLFW_KEY_F8;
		if (strcasecmp(string.c_str(), "f9") == 0) return GLFW_KEY_F9;
		if (strcasecmp(string.c_str(), "f10") == 0) return GLFW_KEY_F10;
		if (strcasecmp(string.c_str(), "f11") == 0) return GLFW_KEY_F11;
		if (strcasecmp(string.c_str(), "f12") == 0) return GLFW_KEY_F12;
		if (strcasecmp(string.c_str(), "f13") == 0) return GLFW_KEY_F13;
		if (strcasecmp(string.c_str(), "f14") == 0) return GLFW_KEY_F14;
		if (strcasecmp(string.c_str(), "f15") == 0) return GLFW_KEY_F15;
		if (strcasecmp(string.c_str(), "f16") == 0) return GLFW_KEY_F16;
		if (strcasecmp(string.c_str(), "f17") == 0) return GLFW_KEY_F17;
		if (strcasecmp(string.c_str(), "f18") == 0) return GLFW_KEY_F18;
		if (strcasecmp(string.c_str(), "f19") == 0) return GLFW_KEY_F19;
		if (strcasecmp(string.c_str(), "f20") == 0) return GLFW_KEY_F20;
		if (strcasecmp(string.c_str(), "f21") == 0) return GLFW_KEY_F21;
		if (strcasecmp(string.c_str(), "f22") == 0) return GLFW_KEY_F22;
		if (strcasecmp(string.c_str(), "f23") == 0) return GLFW_KEY_F23;
		if (strcasecmp(string.c_str(), "f24") == 0) return GLFW_KEY_F24;
		if (strcasecmp(string.c_str(), "f25") == 0) return GLFW_KEY_F25;
		if (strcasecmp(string.c_str(), "kp_0") == 0) return GLFW_KEY_KP_0;
		if (strcasecmp(string.c_str(), "kp_1") == 0) return GLFW_KEY_KP_1;
		if (strcasecmp(string.c_str(), "kp_2") == 0) return GLFW_KEY_KP_2;
		if (strcasecmp(string.c_str(), "kp_3") == 0) return GLFW_KEY_KP_3;
		if (strcasecmp(string.c_str(), "kp_4") == 0) return GLFW_KEY_KP_4;
		if (strcasecmp(string.c_str(), "kp_5") == 0) return GLFW_KEY_KP_5;
		if (strcasecmp(string.c_str(), "kp_6") == 0) return GLFW_KEY_KP_6;
		if (strcasecmp(string.c_str(), "kp_7") == 0) return GLFW_KEY_KP_7;
		if (strcasecmp(string.c_str(), "kp_8") == 0) return GLFW_KEY_KP_8;
		if (strcasecmp(string.c_str(), "kp_9") == 0) return GLFW_KEY_KP_9;
		if (strcasecmp(string.c_str(), "kp_decimal") == 0) return GLFW_KEY_KP_DECIMAL;
		if (strcasecmp(string.c_str(), "kp_divide") == 0) return GLFW_KEY_KP_DIVIDE;
		if (strcasecmp(string.c_str(), "kp_multiply") == 0) return GLFW_KEY_KP_MULTIPLY;
		if (strcasecmp(string.c_str(), "kp_subtract") == 0) return GLFW_KEY_KP_SUBTRACT;
		if (strcasecmp(string.c_str(), "kp_add") == 0) return GLFW_KEY_KP_ADD;
		if (strcasecmp(string.c_str(), "kp_enter") == 0) return GLFW_KEY_KP_ENTER;
		if (strcasecmp(string.c_str(), "kp_equal") == 0) return GLFW_KEY_KP_EQUAL;
		if (strcasecmp(string.c_str(), "left_shift") == 0) return GLFW_KEY_LEFT_SHIFT;
		if (strcasecmp(string.c_str(), "left_control") == 0) return GLFW_KEY_LEFT_CONTROL;
		if (strcasecmp(string.c_str(), "left_alt") == 0) return GLFW_KEY_LEFT_ALT;
		if (strcasecmp(string.c_str(), "left_super") == 0) return GLFW_KEY_LEFT_SUPER;
		if (strcasecmp(string.c_str(), "right_shift") == 0) return GLFW_KEY_RIGHT_SHIFT;
		if (strcasecmp(string.c_str(), "right_control") == 0) return GLFW_KEY_RIGHT_CONTROL;
		if (strcasecmp(string.c_str(), "right_alt") == 0) return GLFW_KEY_RIGHT_ALT;
		if (strcasecmp(string.c_str(), "right_super") == 0) return GLFW_KEY_RIGHT_SUPER;
		if (strcasecmp(string.c_str(), "menu") == 0) return GLFW_KEY_MENU;
		return GLFW_KEY_UNKNOWN;
	}
	
	std::string Settings::GetButtonString(int button)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_1: return "mouse1";
		case GLFW_MOUSE_BUTTON_2: return "mouse2";
		case GLFW_MOUSE_BUTTON_3: return "mouse3";
		case GLFW_MOUSE_BUTTON_4: return "mouse4";
		case GLFW_MOUSE_BUTTON_5: return "mouse5";
		case GLFW_MOUSE_BUTTON_6: return "mouse6";
		case GLFW_MOUSE_BUTTON_7: return "mouse7";
		case GLFW_MOUSE_BUTTON_8: return "mouse8";
		case GLFW_KEY_SPACE: return "space";
		case GLFW_KEY_APOSTROPHE: return "apostrophe";
		case GLFW_KEY_COMMA: return "comma";
		case GLFW_KEY_MINUS: return "minus";
		case GLFW_KEY_PERIOD: return "period";
		case GLFW_KEY_SLASH: return "slash";
		case GLFW_KEY_0: return "0";
		case GLFW_KEY_1: return "1";
		case GLFW_KEY_2: return "2";
		case GLFW_KEY_3: return "3";
		case GLFW_KEY_4: return "4";
		case GLFW_KEY_5: return "5";
		case GLFW_KEY_6: return "6";
		case GLFW_KEY_7: return "7";
		case GLFW_KEY_8: return "8";
		case GLFW_KEY_9: return "9";
		case GLFW_KEY_SEMICOLON: return "semicolon";
		case GLFW_KEY_EQUAL: return "equal";
		case GLFW_KEY_A: return "a";
		case GLFW_KEY_B: return "b";
		case GLFW_KEY_C: return "c";
		case GLFW_KEY_D: return "d";
		case GLFW_KEY_E: return "e";
		case GLFW_KEY_F: return "f";
		case GLFW_KEY_G: return "g";
		case GLFW_KEY_H: return "h";
		case GLFW_KEY_I: return "i";
		case GLFW_KEY_J: return "j";
		case GLFW_KEY_K: return "k";
		case GLFW_KEY_L: return "l";
		case GLFW_KEY_M: return "m";
		case GLFW_KEY_N: return "n";
		case GLFW_KEY_O: return "o";
		case GLFW_KEY_P: return "p";
		case GLFW_KEY_Q: return "q";
		case GLFW_KEY_R: return "r";
		case GLFW_KEY_S: return "s";
		case GLFW_KEY_T: return "t";
		case GLFW_KEY_U: return "u";
		case GLFW_KEY_V: return "v";
		case GLFW_KEY_W: return "w";
		case GLFW_KEY_X: return "x";
		case GLFW_KEY_Y: return "y";
		case GLFW_KEY_Z: return "z";
		case GLFW_KEY_LEFT_BRACKET: return "left_bracket";
		case GLFW_KEY_BACKSLASH: return "backslash";
		case GLFW_KEY_RIGHT_BRACKET: return "right_bracket";
		case GLFW_KEY_GRAVE_ACCENT: return "grave_accent";
		case GLFW_KEY_WORLD_1: return "world_1";
		case GLFW_KEY_WORLD_2: return "world_2";
		case GLFW_KEY_ESCAPE: return "escape";
		case GLFW_KEY_ENTER: return "enter";
		case GLFW_KEY_TAB: return "tab";
		case GLFW_KEY_BACKSPACE: return "backspace";
		case GLFW_KEY_INSERT: return "insert";
		case GLFW_KEY_DELETE: return "delete";
		case GLFW_KEY_RIGHT: return "right";
		case GLFW_KEY_LEFT: return "left";
		case GLFW_KEY_DOWN: return "down";
		case GLFW_KEY_UP: return "up";
		case GLFW_KEY_PAGE_UP: return "page_up";
		case GLFW_KEY_PAGE_DOWN: return "page_down";
		case GLFW_KEY_HOME: return "home";
		case GLFW_KEY_END: return "end";
		case GLFW_KEY_CAPS_LOCK: return "caps_lock";
		case GLFW_KEY_SCROLL_LOCK: return "scroll_lock";
		case GLFW_KEY_NUM_LOCK: return "num_lock";
		case GLFW_KEY_PRINT_SCREEN: return "print_screen";
		case GLFW_KEY_PAUSE: return "pause";
		case GLFW_KEY_F1: return "f1";
		case GLFW_KEY_F2: return "f2";
		case GLFW_KEY_F3: return "f3";
		case GLFW_KEY_F4: return "f4";
		case GLFW_KEY_F5: return "f5";
		case GLFW_KEY_F6: return "f6";
		case GLFW_KEY_F7: return "f7";
		case GLFW_KEY_F8: return "f8";
		case GLFW_KEY_F9: return "f9";
		case GLFW_KEY_F10: return "f10";
		case GLFW_KEY_F11: return "f11";
		case GLFW_KEY_F12: return "f12";
		case GLFW_KEY_F13: return "f13";
		case GLFW_KEY_F14: return "f14";
		case GLFW_KEY_F15: return "f15";
		case GLFW_KEY_F16: return "f16";
		case GLFW_KEY_F17: return "f17";
		case GLFW_KEY_F18: return "f18";
		case GLFW_KEY_F19: return "f19";
		case GLFW_KEY_F20: return "f20";
		case GLFW_KEY_F21: return "f21";
		case GLFW_KEY_F22: return "f22";
		case GLFW_KEY_F23: return "f23";
		case GLFW_KEY_F24: return "f24";
		case GLFW_KEY_F25: return "f25";
		case GLFW_KEY_KP_0: return "kp_0";
		case GLFW_KEY_KP_1: return "kp_1";
		case GLFW_KEY_KP_2: return "kp_2";
		case GLFW_KEY_KP_3: return "kp_3";
		case GLFW_KEY_KP_4: return "kp_4";
		case GLFW_KEY_KP_5: return "kp_5";
		case GLFW_KEY_KP_6: return "kp_6";
		case GLFW_KEY_KP_7: return "kp_7";
		case GLFW_KEY_KP_8: return "kp_8";
		case GLFW_KEY_KP_9: return "kp_9";
		case GLFW_KEY_KP_DECIMAL: return "kp_decimal";
		case GLFW_KEY_KP_DIVIDE: return "kp_divide";
		case GLFW_KEY_KP_MULTIPLY: return "kp_multiply";
		case GLFW_KEY_KP_SUBTRACT: return "kp_subtract";
		case GLFW_KEY_KP_ADD: return "kp_add";
		case GLFW_KEY_KP_ENTER: return "kp_enter";
		case GLFW_KEY_KP_EQUAL: return "kp_equal";
		case GLFW_KEY_LEFT_SHIFT: return "left_shift";
		case GLFW_KEY_LEFT_CONTROL: return "left_control";
		case GLFW_KEY_LEFT_ALT: return "left_alt";
		case GLFW_KEY_LEFT_SUPER: return "left_super";
		case GLFW_KEY_RIGHT_SHIFT: return "right_shift";
		case GLFW_KEY_RIGHT_CONTROL: return "right_control";
		case GLFW_KEY_RIGHT_ALT: return "right_alt";
		case GLFW_KEY_RIGHT_SUPER: return "right_super";
		case GLFW_KEY_MENU: return "menu";
		default: return "unknown";
		}
	}
	
	QualitySettings Settings::ParseQualityString(const std::string& string, QualitySettings def)
	{
		if (strcasecmp(string.c_str(), "low") == 0)
			return QualitySettings::Low;
		if (strcasecmp(string.c_str(), "medium") == 0)
			return QualitySettings::Medium;
		if (strcasecmp(string.c_str(), "high") == 0)
			return QualitySettings::High;
		return def;
	}
	
	std::string Settings::GetQualityString(QualitySettings quality)
	{
		switch (quality)
		{
		case QualitySettings::Low:
			return "low";
		case QualitySettings::Medium:
			return "medium";
		case QualitySettings::High:
			return "high";
		default:
			return "unknown";
		}
	}
}
