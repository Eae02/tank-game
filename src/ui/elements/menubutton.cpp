#include "menubutton.h"
#include "../../updateinfo.h"
#include "../../mouse.h"
#include "../../graphics/ui/uirenderer.h"
#include "../../graphics/ui/font.h"
#include "../../audio/soundeffectplayer.h"

namespace TankGame
{
	MenuButton::MenuButton(std::u32string label)
	{
		SetLabel(std::move(label));
	}
	
	bool MenuButton::Update(const UpdateInfo& updateInfo)
	{
		float targetHoverTransition = 0.0f;
		bool clicked = false;
		
		if (m_area.Contains(updateInfo.m_mouse.pos))
		{
			if (m_hoverTransition == 0.0f)
				PlayMouseOverEffect();
			
			targetHoverTransition = 1.0f;
			
			if (updateInfo.m_mouse.IsDown(MouseButton::Left))
				targetHoverTransition = 0.75f;
			else if (updateInfo.m_mouse.WasDown(MouseButton::Left))
				clicked = true;
		}
		
		float transitionDelta = targetHoverTransition - m_hoverTransition;
		m_hoverTransition += glm::min(updateInfo.m_dt * 10, std::abs(transitionDelta)) * glm::sign(transitionDelta);
		
		return clicked;
	}
	
	void MenuButton::Draw(const UIRenderer& uiRenderer) const
	{
		float scale = glm::mix(0.9f, 1.0f, m_hoverTransition);
		
		glm::vec4 backgroundColor = glm::mix(glm::vec4(ParseColorHexCodeSRGB(0x242424), 0.6f),
		                                     glm::vec4(ParseColorHexCodeSRGB(0xED7F09), 0.9f), m_hoverTransition);
		
		Rectangle rectangle;
		rectangle.w = m_area.w * scale;
		rectangle.h = m_area.h * scale;
		rectangle.x = m_area.x - (rectangle.w - m_area.w) / 2.0f;
		rectangle.y = m_area.y - (rectangle.h - m_area.h) / 2.0f;
		
		uiRenderer.DrawRectangle(rectangle, backgroundColor);
		
		uiRenderer.DrawString(Font::GetNamedFont(FontNames::MenuButtonFont), m_label, rectangle, Alignment::Center,
		                      Alignment::Center, glm::vec4(1.0f), scale);
	}
	
	void MenuButton::SetLabel(std::u32string label)
	{
		m_label = std::move(label);
		
		glm::vec2 size = Font::GetNamedFont(FontNames::MenuButtonFont).MeasureString(m_label);
		m_area.w = glm::max(size.x + 20, 200.0f);
		m_area.h = size.y + 20;
		
		UpdateAreaPosition();
	}
	
	void MenuButton::SetPosition(glm::vec2 position)
	{
		m_centerPos = position;
		UpdateAreaPosition();
	}
	
	glm::vec2 MenuButton::GetSize() const
	{
		return m_area.Size();
	}
	
	void MenuButton::UpdateAreaPosition()
	{
		m_area.x = m_centerPos.x - m_area.w / 2.0f;
		m_area.y = m_centerPos.y - m_area.h / 2.0f;
	}
}
