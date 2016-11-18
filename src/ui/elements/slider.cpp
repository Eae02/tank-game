#include "slider.h"
#include "../../mouse.h"
#include "../../updateinfo.h"
#include "../../graphics/ui/uirenderer.h"
#include "../../graphics/ui/font.h"
#include "../../utils/utils.h"
#include "../../utils/ioutils.h"
#include "../../utils/memory/stackobject.h"

#include <GLFW/glfw3.h>
#include <iostream>

namespace TankGame
{
	const float WIDTH = 200;
	const float HEIGHT = 4;
	const float KNOB_SIZE = 20;
	const float LABEL_SIZE = 35;
	
	StackObject<Texture2D> Slider::s_knobTexture;
	StackObject<Texture2D> Slider::s_labelTexture;
	
	Slider::Slider(float min, float max, float snap)
	    : m_min(min), m_max(max), m_snap(snap / (max - min))
	{
		if (s_knobTexture.IsNull())
		{
			s_knobTexture.Construct(Texture2D::FromFile(GetResDirectory() / "ui" / "slider-knob.png"));
			CallOnClose([] { s_knobTexture.Destroy(); });
		}
		
		if (s_labelTexture.IsNull())
		{
			s_labelTexture.Construct(Texture2D::FromFile(GetResDirectory() / "ui" / "slider-label.png"));
			CallOnClose([] { s_labelTexture.Destroy(); });
		}
	}
	
	bool Slider::Update(const UpdateInfo& updateInfo, float& value)
	{
		Rectangle clickRectangle(m_rectangle.x, m_rectangle.CenterY() - KNOB_SIZE / 2.0f, m_rectangle.w, KNOB_SIZE);
		
		if (clickRectangle.Contains(updateInfo.m_mouse.GetPosition()) &&
		    updateInfo.m_mouse.IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT) &&
		    !updateInfo.m_mouse.WasButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			m_isMoving = true;
		}
		
		if (!updateInfo.m_mouse.IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
			m_isMoving = false;
		
		UpdateTransition(m_labelOpacity, m_isMoving ? 1.0f : 0.0f, updateInfo.m_dt * 10.0f);
		
		if (m_isMoving)
		{
			float oldPosition = m_position;
			
			m_position = glm::clamp((updateInfo.m_mouse.GetX() - m_rectangle.x) / WIDTH, 0.0f, 1.0f);
			m_position = std::round(m_position / m_snap) * m_snap;
			
			value = glm::mix(m_min, m_max, m_position);
			
			return !FloatEqual(oldPosition, m_position);
		}
		
		m_position = (value - m_min) / (m_max - m_min);
		return false;
	}
	
	void Slider::SetPosition(glm::vec2 position)
	{
		m_rectangle = Rectangle::CreateCentered(position, WIDTH, HEIGHT);
	}
	
	glm::vec2 Slider::GetSize() const
	{
		return { WIDTH, glm::max(HEIGHT, KNOB_SIZE) };
	}
	
	void Slider::Draw(const UIRenderer& uiRenderer) const
	{
		uiRenderer.DrawRectangle(m_rectangle, glm::vec4(glm::vec4(ParseColorHexCodeSRGB(0x242424), 0.6f)));
		
		Rectangle knobRectangle = Rectangle::CreateCentered(m_rectangle.x + WIDTH * m_position,
		                                                    m_rectangle.CenterY(), KNOB_SIZE, KNOB_SIZE);
		
		glm::vec3 selectionColor = ParseColorHexCodeSRGB(0xED7F09);
		
		uiRenderer.DrawRectangle(Rectangle(m_rectangle.NearPos(), WIDTH * m_position, m_rectangle.h),
		                         glm::vec4(selectionColor, 1.0f));
		
		if (m_labelOpacity < 1.0f - 1E-6f)
			uiRenderer.DrawSprite(*s_knobTexture, knobRectangle, glm::vec4(selectionColor, 1.0f - m_labelOpacity));
		
		if (m_labelOpacity > 1E-6)
		{
			Rectangle labelRectangle(knobRectangle.CenterX() - LABEL_SIZE / 2.0f, m_rectangle.FarY() + 2,
			                         LABEL_SIZE, LABEL_SIZE * (s_labelTexture->GetHeight() / static_cast<float>(s_labelTexture->GetWidth())));
			
			uiRenderer.DrawSprite(*s_labelTexture, labelRectangle, glm::vec4(selectionColor, m_labelOpacity));
			
			std::string valueString = std::to_string(static_cast<int>(glm::mix(m_min, m_max, m_position)));
			
			Rectangle labelStringRect(labelRectangle.x, labelRectangle.FarY() - LABEL_SIZE, LABEL_SIZE, LABEL_SIZE);
			uiRenderer.DrawString(Font::GetNamedFont(FontNames::StandardUI), valueString, labelStringRect,
			                      Alignment::Center, Alignment::Center, glm::vec4(1));
		}
	}
}
