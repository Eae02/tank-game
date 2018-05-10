#pragma once

#include "iuielement.h"
#include "../../rectangle.h"
#include <functional>

namespace TankGame
{
	class MenuButton : public IUIElement
	{
	public:
		explicit MenuButton(std::u32string label);
		
		bool Update(const class UpdateInfo& updateInfo);
		
		virtual void Draw(const class UIRenderer& uiRenderer) const override;
		
		void SetLabel(std::u32string label);
		inline const std::u32string& GetLabel() const
		{ return m_label; }
		
		virtual void SetPosition(glm::vec2 position) override;
		
		virtual glm::vec2 GetSize() const override;
		
		inline float GetHeight() const
		{ return m_area.h; }
		inline float GetWidth() const
		{ return m_area.w; }
		
		inline void OnShown()
		{ m_hoverTransition = 0; }
		
	private:
		void UpdateAreaPosition();
		
		glm::vec2 m_centerPos;
		
		Rectangle m_area;
		std::u32string m_label;
		
		float m_hoverTransition = 0;
	};
}
