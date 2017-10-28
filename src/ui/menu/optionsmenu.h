#pragma once

#include "../elements/menubutton.h"
#include "../elements/combobox.h"
#include "../elements/slider.h"

#include <functional>
#include <array>

namespace TankGame
{
	class OptionsMenu
	{
	public:
		using ApplyCallback = std::function<void(bool fullscreen, int resX, int resY)>;
		
		OptionsMenu();
		
		void OnResize(int newWidth, int newHeight);
		
		void Update(const class UpdateInfo& updateInfo);
		void Draw(const class UIRenderer& uiRenderer) const;
		
		inline void SetBackCallback(std::function<void()> backCallback)
		{ m_backCallback = std::move(backCallback); }
		
		inline void SetApplyCallback(ApplyCallback applyCallback)
		{ m_applyCallback = std::move(applyCallback); }
		
		void OnOpen();
		
	private:
		static constexpr size_t NUM_SECTIONS = 3;
		static const std::array<std::u32string, NUM_SECTIONS> SECTION_TITLES;
		
		static constexpr float CONTENTS_HEIGHT = 700;
		
		void LayoutContentWidgets();
		
		struct Label
		{
			std::u32string m_string;
			Rectangle m_rectangle;
			
			inline explicit Label(std::u32string string)
			    : m_string(string + U":") { }
		};
		
		std::array<Label, 13> m_settingLabels;
		
		std::array<Rectangle, NUM_SECTIONS> m_sectionTitleRectangles;
		
		std::function<void()> m_backCallback;
		ApplyCallback m_applyCallback;
		
		Rectangle m_scissorArea;
		
		float m_scroll = 0;
		float m_maxScroll = 0;
		
		float m_contentsBeginY = 0;
		float m_contentsBeginX = 0;
		
		MenuButton m_backButton;
		MenuButton m_applyButton;
		
		long m_displayModeIndex;
		ComboBox m_displayModeComboBox;
		
		long m_currentResolutionIndex;
		ComboBox m_resolutionsComboBox;
		
		ComboBox m_vSyncComboBox;
		
		Slider m_gammaSlider;
		
		ComboBox m_resolutionScaleComboBox;
		
		ComboBox m_lightingQualityComboBox;
		ComboBox m_particlesQualityComboBox;
		ComboBox m_postQualityComboBox;
		ComboBox m_bloomComboBox;
		ComboBox m_frameQueueComboBox;
		
		Slider m_masterVolumeSlider;
		Slider m_musicVolumeSlider;
		Slider m_sfxVolumeSlider;
	};
}
