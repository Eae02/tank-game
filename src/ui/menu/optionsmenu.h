#pragma once

#include "../elements/menubutton.h"
#include "../elements/combobox.h"
#include "../elements/slider.h"

#include <functional>
#include <optional>
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
		
		void OnOpen();
		
		std::function<void()> m_backCallback;
		ApplyCallback m_applyCallback;
		
	private:
		static constexpr float CONTENTS_HEIGHT = 700;
		
		void LayoutContentWidgets();
		
		struct Label
		{
			std::string string;
			Rectangle rectangle;
		};
		
		size_t m_numSettingLabels = 0;
		std::array<Label, 13> m_settingLabels;
		std::array<Label, 3> m_sectionTitleLabels;
		
		Rectangle m_scissorArea;
		
		float m_scroll = 0;
		float m_maxScroll = 0;
		
		float m_contentsBeginY = 0;
		float m_contentsBeginX = 0;
		
		MenuButton m_backButton;
		MenuButton m_applyButton;
		
		long m_displayModeIndex = 0;
		std::optional<ComboBox> m_displayModeComboBox;
		
		long m_currentResolutionIndex = -1;
		std::optional<ComboBox> m_resolutionsComboBox;
		
		ComboBox m_vSyncComboBox;
		
		Slider m_gammaSlider;
		
		ComboBox m_resolutionScaleComboBox;
		
		ComboBox m_lightingQualityComboBox;
		ComboBox m_particlesQualityComboBox;
		ComboBox m_postQualityComboBox;
		ComboBox m_bloomComboBox;
		
		Slider m_masterVolumeSlider;
		Slider m_musicVolumeSlider;
		Slider m_sfxVolumeSlider;
	};
}
