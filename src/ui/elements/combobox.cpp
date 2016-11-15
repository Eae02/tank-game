#include "combobox.h"
#include "../../updateinfo.h"
#include "../../graphics/ui/uirenderer.h"
#include "../../graphics/ui/font.h"
#include "../../graphics/scissor.h"
#include "../../mouse.h"
#include "../../keyboard.h"

#include <GLFW/glfw3.h>
#include <algorithm>

namespace TankGame
{
	const int MAX_ITEMS = 4;
	const float DROP_DOWN_ITEM_HEIGHT = 25.0f;
	
	constexpr float ComboBox::HEIGHT;
	
	ComboBox::ComboBox(std::initializer_list<std::u32string> entries)
	    : m_entries(entries)
	{
		m_scrollBar.SetContentSettings(m_entries.size(), MAX_ITEMS);
	}
	
	void ComboBox::AddEntry(std::u32string entry)
	{
		m_entries.emplace_back(std::move(entry));
		m_scrollBar.SetContentSettings(m_entries.size(), MAX_ITEMS);
	}
	
	void ComboBox::SetPosition(glm::vec2 position)
	{
		m_area = Rectangle::CreateCentered(position, 200, HEIGHT);
		
		float dropDownHeight = std::min<int>(m_entries.size(), MAX_ITEMS) * DROP_DOWN_ITEM_HEIGHT;
		m_dropDownArea = Rectangle(m_area.x, m_area.y - dropDownHeight, m_area.w, dropDownHeight);
		
		m_scrollBar.SetPositionAndHeight({ m_dropDownArea.FarX() - ScrollBar::WIDTH, m_dropDownArea.y }, m_dropDownArea.h);
	}
	
	bool ComboBox::Update(const UpdateInfo& updateInfo, long& currentItem)
	{
		bool changed = false;
		
		bool clicked = updateInfo.m_mouse.IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT) &&
		               !updateInfo.m_mouse.WasButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
		
		float targetHoverTransition = 0.0f;
		
		if (m_isDropDownShown)
		{
			targetHoverTransition = 1.0f;
			
			if (updateInfo.m_keyboard.IsKeyDown(GLFW_KEY_ESCAPE) && !updateInfo.m_keyboard.WasKeyDown(GLFW_KEY_ESCAPE))
			{
				m_hoveredItem = -1;
				m_isDropDownShown = false;
			}
			else if (m_dropDownArea.Contains(updateInfo.m_mouse.GetPosition()))
			{
				SetScroll(m_dropDownScroll - updateInfo.m_mouse.GetDeltaScroll() * 0.5f);
				
				m_hoveredItem = ((m_dropDownArea.FarY() - updateInfo.m_mouse.GetPosition().y) / DROP_DOWN_ITEM_HEIGHT) + m_dropDownScroll;
				
				if (!updateInfo.m_mouse.IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT) &&
				    updateInfo.m_mouse.WasButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
				{
					currentItem = m_hoveredItem;
					changed = true;
					m_isDropDownShown = false;
				}
			}
			else
			{
				m_hoveredItem = -1;
				if (clicked)
					m_isDropDownShown = false;
			}
			
			m_scrollBar.Update(updateInfo, m_dropDownScroll);
		}
		else
		{
			if (m_area.Contains(updateInfo.m_mouse.GetPosition()))
			{
				targetHoverTransition = 1.0f;
				
				if (clicked)
				{
					m_isDropDownShown = true;
					m_hoveredItem = -1;
					SetScroll(currentItem + 0.5f - MAX_ITEMS / 2.0f);
				}
			}
		}
		
		float transitionDelta = targetHoverTransition - m_hoverTransition;
		m_hoverTransition += glm::min(updateInfo.m_dt * 10, std::abs(transitionDelta)) * glm::sign(transitionDelta);
		
		m_currentItem = currentItem;
		
		return changed;
	}
	
	void ComboBox::Draw(const UIRenderer& uiRenderer) const
	{
		glm::vec4 backgroundColor = glm::mix(glm::vec4(ParseColorHexCodeSRGB(0x242424), 0.6f),
		                                     glm::vec4(ParseColorHexCodeSRGB(0xED7F09), 0.9f), m_hoverTransition);
		
		uiRenderer.DrawRectangle(m_area, backgroundColor);
		
		if (m_currentItem >= 0)
		{
			Rectangle textArea = m_area;
			textArea.w -= 30;
			
			PushScissorRect(textArea);
			
			uiRenderer.DrawString(Font::GetNamedFont(FontNames::StandardUI), m_entries[m_currentItem], textArea,
			                      Alignment::Left, Alignment::Center, glm::vec4(1.0f));
			
			PopScissorRect();
		}
		
		if (m_isDropDownShown)
		{
			PushScissorRect(m_dropDownArea);
			
			uiRenderer.DrawRectangle(m_dropDownArea, glm::vec4(ParseColorHexCodeSRGB(0x4F4F4F), 1.0f));
			
			for (size_t i = 0; i < m_entries.size(); i++)
			{
				float offsetY = i - m_dropDownScroll + 1;
				Rectangle entryRect(m_dropDownArea.x, m_dropDownArea.FarY() - offsetY * DROP_DOWN_ITEM_HEIGHT,
				                    m_dropDownArea.w, DROP_DOWN_ITEM_HEIGHT);
				
				if (static_cast<long>(i) == m_hoveredItem)
					uiRenderer.DrawRectangle(entryRect, glm::vec4(ParseColorHexCodeSRGB(0xED7F09), 1.0f));
				else if (static_cast<long>(i) == m_currentItem)
					uiRenderer.DrawRectangle(entryRect, glm::vec4(ParseColorHexCodeSRGB(0x363636), 1.0f));
				
				uiRenderer.DrawString(Font::GetNamedFont(FontNames::StandardUI), m_entries[i], entryRect,
				                      Alignment::Left, Alignment::Center, glm::vec4(1.0f));
			}
			
			m_scrollBar.Draw(uiRenderer);
			
			PopScissorRect();
		}
	}
	
	glm::vec2 ComboBox::GetSize() const
	{
		return { 200.0f, HEIGHT };
	}
	
	void ComboBox::SetScroll(float scroll)
	{
		float maxScroll = glm::max<int>(m_entries.size() - MAX_ITEMS, 0);
		m_dropDownScroll = glm::clamp(scroll, 0.0f, maxScroll);
	}
}
