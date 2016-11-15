#include "scrollbar.h"
#include "../../updateinfo.h"
#include "../../graphics/ui/uirenderer.h"

namespace TankGame
{
	constexpr float ScrollBar::WIDTH;
	
	ScrollBar::ScrollBar()
	{
		
	}
	
	bool ScrollBar::Update(const UpdateInfo& updateInfo, float& scroll)
	{
		if (m_paneHeight + 1E-6f > m_contentsHeight)
			return false;
		
		m_scrollBarPosition = scroll / (m_contentsHeight - m_paneHeight);
	}
	
	void ScrollBar::Draw(const UIRenderer& uiRenderer) const
	{
		if (m_paneHeight + 1E-6f < m_contentsHeight)
			uiRenderer.DrawRectangle(GetScrollBarRectangle(), glm::vec4(1.0f));
	}
	
	void ScrollBar::SetPositionAndHeight(glm::vec2 position, float height)
	{
		m_area = { position.x, position.y, WIDTH, height };
		UpdateScrollBarHeight();
	}
	
	void ScrollBar::SetContentSettings(float contentsHeight, float paneHeight)
	{
		m_contentsHeight = contentsHeight;
		m_paneHeight = paneHeight;
		UpdateScrollBarHeight();
	}
	
	Rectangle ScrollBar::GetScrollBarRectangle() const
	{
		return { m_area.x, m_area.FarY() - m_scrollBarHeight - m_scrollBarPosition *
		        (m_area.h - m_scrollBarHeight), m_area.w, m_scrollBarHeight };
	}
	
	void ScrollBar::UpdateScrollBarHeight()
	{
		m_scrollBarHeight = (m_paneHeight / m_contentsHeight) * m_area.h;
	}
}
