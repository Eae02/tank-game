#include "drawbutton.h"
#include "font.h"
#include "../../utils/utils.h"
#include "../../utils/ioutils.h"

namespace TankGame
{
	static std::unique_ptr<Texture2D> blankButtonTexture;
	
	void DrawButton(const std::u32string& text, UIRenderer& uiRenderer, glm::vec2 pos, float height, float opacity)
	{
		if (blankButtonTexture== nullptr)
		{
			blankButtonTexture = std::make_unique<Texture2D>(Texture2D::FromFile(resDirectoryPath / "ui" / "key.png"));
			CallOnClose([] { blankButtonTexture = nullptr; });
		}
		
		const float WIDTH = 1;
		
		Rectangle rectangle = Rectangle::CreateCentered(pos, WIDTH * height, height);
		
		uiRenderer.DrawSprite(*blankButtonTexture, rectangle, glm::vec4(opacity));
		
		const Font& font = Font::GetNamedFont(FontNames::ButtonFont);
		
		uiRenderer.DrawString(font, text, rectangle, Alignment::Center, Alignment::Center,
		                      glm::vec4(glm::vec3(0.3f), opacity), height / (2.0f * font.GetSize()));
	}
}
