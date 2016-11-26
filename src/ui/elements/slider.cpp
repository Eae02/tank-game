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
	
	float Slider::s_labelTextureAR;
	float Slider::s_labelStringAR;
	
	Slider::Slider(float min, float max, float snap)
	    : m_min(min), m_max(max), m_snap(snap / (max - min)), m_roundExp(FloatEqual(snap, 0.0f) ? 1.0f : snap)
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
			
			s_labelTextureAR = s_labelTexture->GetHeight() / static_cast<float>(s_labelTexture->GetWidth());;
			s_labelStringAR = (s_labelTexture->GetHeight() - 16.0f) / static_cast<float>(s_labelTexture->GetWidth());
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
		
		UpdateTransition(m_grabbedTProgress, m_isMoving ? 1.0f : 0.0f, updateInfo.m_dt * 10.0f);
		
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
		m_rectangle = Rectangle::CreateCentered(position, WIDTH, HEIGHT + 10.0f);
		m_rectangle.h -= 10.0f;
	}
	
	glm::vec2 Slider::GetSize() const
	{
		return { WIDTH, glm::max(HEIGHT, KNOB_SIZE) + 10.0f };
	}
	
	void Slider::Draw(const UIRenderer& uiRenderer) const
	{
		uiRenderer.DrawRectangle(m_rectangle, glm::vec4(glm::vec4(ParseColorHexCodeSRGB(0x242424), 0.6f)));
		
		const glm::vec3 selectionColor = ParseColorHexCodeSRGB(0xED7F09);
		uiRenderer.DrawRectangle(Rectangle(m_rectangle.NearPos(), WIDTH * m_position, m_rectangle.h),
		                         glm::vec4(selectionColor, 1.0f));
		
		float knobSize = KNOB_SIZE * (1.0f - m_grabbedTProgress);
		Rectangle knobRectangle = Rectangle::CreateCentered(m_rectangle.x + WIDTH * m_position,
		                                                    m_rectangle.CenterY(), knobSize, knobSize);
		
		if (m_grabbedTProgress < 1.0f - 1E-6f)
			uiRenderer.DrawSprite(*s_knobTexture, knobRectangle, glm::vec4(selectionColor, 1.0f - m_grabbedTProgress));
		
		if (m_grabbedTProgress > 1E-6)
		{
			Rectangle labelRectangle(knobRectangle.CenterX() - LABEL_SIZE / 2.0f, m_rectangle.FarY() + 2,
			                         LABEL_SIZE, LABEL_SIZE * s_labelTextureAR);
			
			uiRenderer.DrawSprite(*s_labelTexture, labelRectangle, glm::vec4(selectionColor, m_grabbedTProgress));
			
			std::ostringstream valueSS;
			valueSS << std::round(glm::mix(m_min, m_max, m_position) / m_roundExp) * m_roundExp;
			
			float labelStringH = s_labelStringAR * LABEL_SIZE;
			
			Rectangle labelStringRect(labelRectangle.x, labelRectangle.FarY() - labelStringH, LABEL_SIZE, labelStringH);
			uiRenderer.DrawString(Font::GetNamedFont(FontNames::StandardUI), valueSS.str(), labelStringRect,
			                      Alignment::Center, Alignment::Center, glm::vec4(1));
		}
	}
}
