#pragma once

#include "../../rectangle.h"

namespace TankGame
{
	class ScrollBar
	{
	public:
		ScrollBar();
		
		bool Update(const class UpdateInfo& updateInfo, float& scroll);
		
		void Draw(const class UIRenderer& uiRenderer) const;
		
		void SetPositionAndHeight(glm::vec2 position, float height);
		
		void SetContentSettings(float contentsHeight, float paneHeight);
		
		static constexpr float WIDTH = 2;
		
	private:
		Rectangle GetScrollBarRectangle() const;
		
		void UpdateScrollBarHeight();
		
		float m_contentsHeight = 0;
		float m_paneHeight = 0;
		
		float m_scrollBarHeight;
		
		float m_scrollBarPosition;
		
		Rectangle m_area;
	};
}
