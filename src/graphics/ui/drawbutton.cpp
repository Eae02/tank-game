#include "drawbutton.h"
#include "font.h"
#include "../../utils/utils.h"
#include "../../utils/ioutils.h"

#include <GLFW/glfw3.h>

namespace TankGame
{
	static std::u32string GetButtonString(int button)
	{
		switch (button)
		{
		case GLFW_KEY_SPACE: return U"Space";
		case GLFW_KEY_APOSTROPHE: return U"'";
		case GLFW_KEY_COMMA: return U",";
		case GLFW_KEY_MINUS: return U"-";
		case GLFW_KEY_PERIOD: return U".";
		case GLFW_KEY_SLASH: return U"/";
		case GLFW_KEY_0: return U"0";
		case GLFW_KEY_1: return U"1";
		case GLFW_KEY_2: return U"2";
		case GLFW_KEY_3: return U"3";
		case GLFW_KEY_4: return U"4";
		case GLFW_KEY_5: return U"5";
		case GLFW_KEY_6: return U"6";
		case GLFW_KEY_7: return U"7";
		case GLFW_KEY_8: return U"8";
		case GLFW_KEY_9: return U"9";
		case GLFW_KEY_SEMICOLON: return U";";
		case GLFW_KEY_EQUAL: return U"=";
		case GLFW_KEY_A: return U"A";
		case GLFW_KEY_B: return U"B";
		case GLFW_KEY_C: return U"C";
		case GLFW_KEY_D: return U"D";
		case GLFW_KEY_E: return U"E";
		case GLFW_KEY_F: return U"F";
		case GLFW_KEY_G: return U"G";
		case GLFW_KEY_H: return U"H";
		case GLFW_KEY_I: return U"I";
		case GLFW_KEY_J: return U"J";
		case GLFW_KEY_K: return U"K";
		case GLFW_KEY_L: return U"L";
		case GLFW_KEY_M: return U"M";
		case GLFW_KEY_N: return U"N";
		case GLFW_KEY_O: return U"O";
		case GLFW_KEY_P: return U"P";
		case GLFW_KEY_Q: return U"Q";
		case GLFW_KEY_R: return U"R";
		case GLFW_KEY_S: return U"S";
		case GLFW_KEY_T: return U"T";
		case GLFW_KEY_U: return U"U";
		case GLFW_KEY_V: return U"V";
		case GLFW_KEY_W: return U"W";
		case GLFW_KEY_X: return U"X";
		case GLFW_KEY_Y: return U"Y";
		case GLFW_KEY_Z: return U"Z";
		case GLFW_KEY_LEFT_BRACKET: return U"[";
		case GLFW_KEY_BACKSLASH: return U"\\";
		case GLFW_KEY_RIGHT_BRACKET: return U"]";
		case GLFW_KEY_GRAVE_ACCENT: return U"~";
		case GLFW_KEY_ESCAPE: return U"Esc";
		case GLFW_KEY_ENTER: return U"Enter";
		case GLFW_KEY_TAB: return U"Tab";
		case GLFW_KEY_BACKSPACE: return U"Back";
		case GLFW_KEY_INSERT: return U"Insert";
		case GLFW_KEY_DELETE: return U"Delete";
		case GLFW_KEY_RIGHT: return U"Right";
		case GLFW_KEY_LEFT: return U"Left";
		case GLFW_KEY_DOWN: return U"Down";
		case GLFW_KEY_UP: return U"Up";
		case GLFW_KEY_PAGE_UP: return U"Pg Up";
		case GLFW_KEY_PAGE_DOWN: return U"Pg Dn";
		case GLFW_KEY_HOME: return U"Home";
		case GLFW_KEY_END: return U"End";
		case GLFW_KEY_CAPS_LOCK: return U"Caps Lock";
		case GLFW_KEY_SCROLL_LOCK: return U"Scroll Lock";
		case GLFW_KEY_NUM_LOCK: return U"num_lock";
		case GLFW_KEY_PRINT_SCREEN: return U"print_screen";
		case GLFW_KEY_PAUSE: return U"Pause";
		case GLFW_KEY_F1: return U"F1";
		case GLFW_KEY_F2: return U"F2";
		case GLFW_KEY_F3: return U"F3";
		case GLFW_KEY_F4: return U"F4";
		case GLFW_KEY_F5: return U"F5";
		case GLFW_KEY_F6: return U"F6";
		case GLFW_KEY_F7: return U"F7";
		case GLFW_KEY_F8: return U"F8";
		case GLFW_KEY_F9: return U"F9";
		case GLFW_KEY_F10: return U"F10";
		case GLFW_KEY_F11: return U"F11";
		case GLFW_KEY_F12: return U"F12";
		case GLFW_KEY_F13: return U"F13";
		case GLFW_KEY_F14: return U"F14";
		case GLFW_KEY_F15: return U"F15";
		case GLFW_KEY_F16: return U"F16";
		case GLFW_KEY_F17: return U"F17";
		case GLFW_KEY_F18: return U"F18";
		case GLFW_KEY_F19: return U"F19";
		case GLFW_KEY_F20: return U"F20";
		case GLFW_KEY_F21: return U"F21";
		case GLFW_KEY_F22: return U"F22";
		case GLFW_KEY_F23: return U"F23";
		case GLFW_KEY_F24: return U"F24";
		case GLFW_KEY_F25: return U"F25";
		case GLFW_KEY_KP_0: return U"Numpad 0";
		case GLFW_KEY_KP_1: return U"Numpad 1";
		case GLFW_KEY_KP_2: return U"Numpad 2";
		case GLFW_KEY_KP_3: return U"Numpad 3";
		case GLFW_KEY_KP_4: return U"Numpad 4";
		case GLFW_KEY_KP_5: return U"Numpad 5";
		case GLFW_KEY_KP_6: return U"Numpad 6";
		case GLFW_KEY_KP_7: return U"Numpad 7";
		case GLFW_KEY_KP_8: return U"Numpad 8";
		case GLFW_KEY_KP_9: return U"Numpad 9";
		case GLFW_KEY_KP_DECIMAL: return U"Numpad .";
		case GLFW_KEY_KP_DIVIDE: return U"Numpad /";
		case GLFW_KEY_KP_MULTIPLY: return U"Numpad *";
		case GLFW_KEY_KP_SUBTRACT: return U"Numpad -";
		case GLFW_KEY_KP_ADD: return U"Numpad +";
		case GLFW_KEY_KP_ENTER: return U"Numpad Enter";
		case GLFW_KEY_KP_EQUAL: return U"Numpad =";
		case GLFW_KEY_LEFT_SHIFT: return U"Shift L";
		case GLFW_KEY_LEFT_CONTROL: return U"Ctrl L";
		case GLFW_KEY_LEFT_ALT: return U"Alt L";
		case GLFW_KEY_LEFT_SUPER: return U"Super L";
		case GLFW_KEY_RIGHT_SHIFT: return U"Shift R";
		case GLFW_KEY_RIGHT_CONTROL: return U"Control R";
		case GLFW_KEY_RIGHT_ALT: return U"Alt R";
		case GLFW_KEY_RIGHT_SUPER: return U"Super R";
		case GLFW_KEY_MENU: return U"Menu";
		default: return U"??";
		}
	}
	
	static StackObject<Texture2D> blankButtonTexture;
	
	void DrawButton(int button, UIRenderer& uiRenderer, glm::vec2 pos, float height, float opacity)
	{
		if (blankButtonTexture.IsNull())
		{
			blankButtonTexture.Construct(Texture2D::FromFile(GetResDirectory() / "ui" / "key.png"));
			CallOnClose([] { blankButtonTexture.Destroy(); });
		}
		
		const float WIDTH = 1;
		
		Rectangle rectangle = Rectangle::CreateCentered(pos, WIDTH * height, height);
		
		uiRenderer.DrawSprite(*blankButtonTexture, rectangle, glm::vec4(opacity));
		
		const Font& font = Font::GetNamedFont(FontNames::ButtonFont);
		
		std::u32string string = GetButtonString(button);
		uiRenderer.DrawString(font, string, rectangle, Alignment::Center, Alignment::Center,
		                      glm::vec4(glm::vec3(0.3f), opacity), height / (2.0f * font.GetSize()));
	}
}
