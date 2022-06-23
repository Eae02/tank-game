#pragma once

#include <initializer_list>
#include <string>
#include <vector>

#include "scrollbar.h"
#include "iuielement.h"
#include "../../rectangle.h"

namespace TankGame
{
	class ComboBox : public IUIElement
	{
	public:
		ComboBox() = default;
		ComboBox(std::initializer_list<std::string> entries);
		
		void AddEntry(std::string entry);
		
		virtual void SetPosition(glm::vec2 position) override;
		
		bool Update(const class UpdateInfo& updateInfo, long& currentItem);
		virtual void Draw(const class UIRenderer& uiRenderer) const override;
		
		virtual glm::vec2 GetSize() const override;
		
		inline bool IsDropDownShown() const
		{ return m_isDropDownShown; }
		
		static constexpr float HEIGHT = 30;
		
	private:
		void SetScroll(float scroll);
		void AreaChanged();
		
		ScrollBar m_scrollBar;
		
		std::vector<std::string> m_entries;
		
		Rectangle m_area;
		Rectangle m_dropDownArea;
		
		float m_hoverTransition = 0;
		
		float m_dropDownScroll = 0;
		
		bool m_isDropDownShown = false;
		
		long m_hoveredItem = -1;
		long m_currentItem = -1;
	};
}
