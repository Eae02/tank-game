#include "optionsmenu.h"
#include "../../updateinfo.h"
#include "../../graphics/ui/uirenderer.h"
#include "../../graphics/ui/font.h"
#include "../../graphics/scissor.h"
#include "../../settings.h"
#include "../../platform/common.h"
#include "../../mouse.h"
#include "../../utils/utils.h"

#include <variant>
#include <sstream>

namespace TankGame
{
	static ComboBox CreateQualityComboBox()
	{
		return ComboBox{ "Low", "Medium", "High" };
	}
	
	static ComboBox CreateBoolComboBox()
	{
		return ComboBox{ "Disabled", "Enabled" };
	}
	
	OptionsMenu::OptionsMenu()
	    : m_backButton("Back"), m_applyButton("Apply"),
	      m_vSyncComboBox(CreateBoolComboBox()), m_gammaSlider(0.5f, 1.5f, 0.05f),
	      m_resolutionScaleComboBox{ "50%", "75%", "100%", "125%", "150%", "200%" },
	      m_lightingQualityComboBox(CreateQualityComboBox()), m_particlesQualityComboBox(CreateQualityComboBox()),
	      m_postQualityComboBox(CreateQualityComboBox()), m_bloomComboBox(CreateBoolComboBox()),
	      m_masterVolumeSlider(0, 100, 5), m_musicVolumeSlider(0, 100, 5), m_sfxVolumeSlider(0, 100, 5)
	{
		m_displayModeIndex = Settings::instance.IsFullscreen() ? 0 : 1;
		m_currentResolutionIndex = Settings::instance.GetResolutionIndex();
	}
	
	void OptionsMenu::OnResize(int newWidth, int newHeight)
	{
		m_backButton.SetPosition(glm::vec2(newWidth / 2.0f - 100, 100));
		m_applyButton.SetPosition(glm::vec2(newWidth / 2.0f + 100, 100));
		
		m_contentsBeginY = newHeight - 100;
		m_contentsBeginX = newWidth / 2.0f;
		
		m_scissorArea = { 0, 120, static_cast<float>(newWidth), m_contentsBeginY - 120 };
		
		m_maxScroll = glm::max(CONTENTS_HEIGHT - m_scissorArea.h, 0.0f);
		if (m_scroll > m_maxScroll)
			m_scroll = m_maxScroll;
		
		LayoutContentWidgets();
	}
	
	void OptionsMenu::Update(const UpdateInfo& updateInfo)
	{
		if (!updateInfo.m_videoModes->resolutions.empty() && !m_resolutionsComboBox)
		{
			m_displayModeComboBox = ComboBox{ "Fullscreen", "Windowed" };
			m_resolutionsComboBox = ComboBox();
			for (const glm::ivec2& res : updateInfo.m_videoModes->resolutions)
			{
				std::ostringstream labelStream;
				labelStream << res.x << "x" << res.y;
				m_resolutionsComboBox->AddEntry(labelStream.str());
			}
			LayoutContentWidgets();
		}
		
		if (m_backButton.Update(updateInfo) && m_backCallback != nullptr)
			m_backCallback();
		
		if (m_applyButton.Update(updateInfo) && m_applyCallback != nullptr)
		{
			glm::ivec2 res(-1);
			if (updateInfo.m_videoModes && m_currentResolutionIndex >= 0)
			{
				res = updateInfo.m_videoModes->resolutions[m_currentResolutionIndex];
				Settings::instance.SetResolution(m_currentResolutionIndex, res);
			}
			bool isFullscreen = m_displayModeIndex == 0;
			Settings::instance.SetIsFullscreen(isFullscreen);
			m_applyCallback(m_displayModeIndex == 0, res.x, res.y);
		}
		
		bool anyDropDownShown = 
		        (m_resolutionsComboBox && m_resolutionsComboBox->IsDropDownShown()) ||
		        (m_displayModeComboBox && m_displayModeComboBox->IsDropDownShown()) ||
		        (PLATFORM_CAN_CHANGE_VSYNC && m_vSyncComboBox.IsDropDownShown()) ||
		        m_resolutionScaleComboBox.IsDropDownShown() ||
		        m_lightingQualityComboBox.IsDropDownShown() ||
		        m_particlesQualityComboBox.IsDropDownShown() ||
		        m_postQualityComboBox.IsDropDownShown() ||
		        m_bloomComboBox.IsDropDownShown();
		
		if (!anyDropDownShown && std::abs(updateInfo.m_mouse.GetDeltaScroll()) > 1E-6f)
		{
			m_scroll = glm::clamp(m_scroll - updateInfo.m_mouse.GetDeltaScroll() * 20, 0.0f, m_maxScroll);
			LayoutContentWidgets();
		}
		
		if (m_resolutionsComboBox && (!anyDropDownShown || m_resolutionsComboBox->IsDropDownShown()))
		{
			m_resolutionsComboBox->Update(updateInfo, m_currentResolutionIndex);
		}
		
		if (m_displayModeComboBox && (!anyDropDownShown || m_displayModeComboBox->IsDropDownShown()))
		{
			m_displayModeComboBox->Update(updateInfo, m_displayModeIndex);
		}
		
		if (PLATFORM_CAN_CHANGE_VSYNC && (!anyDropDownShown || m_vSyncComboBox.IsDropDownShown()))
		{
			long vSyncEnabled = Settings::instance.EnableVSync();
			if (m_vSyncComboBox.Update(updateInfo, vSyncEnabled))
				Settings::instance.SetEnableVSync(vSyncEnabled);
		}
		
		if (!anyDropDownShown)
		{
			float gamma = Settings::instance.GetGamma();
			if (m_gammaSlider.Update(updateInfo, gamma))
				Settings::instance.SetGamma(gamma);
		}
		
		if (!anyDropDownShown || m_resolutionScaleComboBox.IsDropDownShown())
		{
			const std::array<ResolutionScales, 6> resScales =
			{
				ResolutionScales::_50,
				ResolutionScales::_75,
				ResolutionScales::_100,
				ResolutionScales::_125,
				ResolutionScales::_150,
				ResolutionScales::_200
			};
			
			auto resScaleIt = std::find(resScales.begin(), resScales.end(),
			                            Settings::instance.GetResolutionScale());
			
			if (resScaleIt == resScales.end())
				resScaleIt = std::find(resScales.begin(), resScales.end(), ResolutionScales::_100);
			
			long resScaleIndex = resScaleIt - resScales.begin();
			
			if (m_resolutionScaleComboBox.Update(updateInfo, resScaleIndex))
				Settings::instance.SetResolutionScale(resScales[resScaleIndex]);
		}
		
		if (!anyDropDownShown || m_lightingQualityComboBox.IsDropDownShown())
		{
			long lightingQuality = static_cast<long>(Settings::instance.GetLightingQuality());
			if (m_lightingQualityComboBox.Update(updateInfo, lightingQuality))
				Settings::instance.SetLightingQuality(static_cast<QualitySettings>(lightingQuality));
		}
		
		if (!anyDropDownShown || m_particlesQualityComboBox.IsDropDownShown())
		{
			long particlesQuality = static_cast<long>(Settings::instance.GetParticleQuality());
			if (m_particlesQualityComboBox.Update(updateInfo, particlesQuality))
				Settings::instance.SetParticleQuality(static_cast<QualitySettings>(particlesQuality));
		}
		
		if (!anyDropDownShown || m_postQualityComboBox.IsDropDownShown())
		{
			long postQuality = static_cast<long>(Settings::instance.GetPostProcessingQuality());
			if (m_postQualityComboBox.Update(updateInfo, postQuality))
				Settings::instance.SetPostProcessingQuality(static_cast<QualitySettings>(postQuality));
		}
		
		if (!anyDropDownShown || m_bloomComboBox.IsDropDownShown())
		{
			long bloomEnabled = Settings::instance.EnableBloom();
			if (m_bloomComboBox.Update(updateInfo, bloomEnabled))
				Settings::instance.SetEnableBloom(bloomEnabled);
		}
		
		if (!anyDropDownShown)
		{
			float volume = Settings::instance.GetMasterVolume() * 100.0f;
			if (m_masterVolumeSlider.Update(updateInfo, volume))
				Settings::instance.SetMasterVolume(volume / 100.0f);
		}
		
		if (!anyDropDownShown)
		{
			float volume = Settings::instance.GetMusicVolume() * 100.0f;
			if (m_musicVolumeSlider.Update(updateInfo, volume))
				Settings::instance.SetMusicVolume(volume / 100.0f);
		}
		
		if (!anyDropDownShown)
		{
			float volume = Settings::instance.GetSFXVolume() * 100.0f;
			if (m_sfxVolumeSlider.Update(updateInfo, volume))
				Settings::instance.SetSFXVolume(volume / 100.0f);
		}
	}
	
	void OptionsMenu::OnOpen()
	{
		m_backButton.OnShown();
		m_applyButton.OnShown();
	}
	
	void OptionsMenu::Draw(const UIRenderer& uiRenderer) const
	{
		m_backButton.Draw(uiRenderer);
		m_applyButton.Draw(uiRenderer);
		
		PushScissorRect(m_scissorArea);
		
		for (const Label& label : m_sectionTitleLabels)
		{
			uiRenderer.DrawString(Font::GetNamedFont(FontNames::MenuTitle), label.string, label.rectangle,
			                      Alignment::Center, Alignment::Center, glm::vec4(1.0f));
		}
		
		std::for_each_n(m_settingLabels.begin(), m_numSettingLabels, [&] (const Label& label)
		{
			uiRenderer.DrawString(Font::GetNamedFont(FontNames::StandardUI), label.string, label.rectangle,
			                      Alignment::Right, Alignment::Center, glm::vec4(1.0f));
		});
		
		//It is important to draw elements in reverse order!
		m_sfxVolumeSlider.Draw(uiRenderer);
		m_musicVolumeSlider.Draw(uiRenderer);
		m_masterVolumeSlider.Draw(uiRenderer);
		m_bloomComboBox.Draw(uiRenderer);
		m_postQualityComboBox.Draw(uiRenderer);
		m_particlesQualityComboBox.Draw(uiRenderer);
		m_lightingQualityComboBox.Draw(uiRenderer);
		m_resolutionScaleComboBox.Draw(uiRenderer);
		m_gammaSlider.Draw(uiRenderer);
		if (PLATFORM_CAN_CHANGE_VSYNC)
			m_vSyncComboBox.Draw(uiRenderer);
		if (m_resolutionsComboBox)
			m_resolutionsComboBox->Draw(uiRenderer);
		if (m_displayModeComboBox)
			m_displayModeComboBox->Draw(uiRenderer);
		
		PopScissorRect();
	}
	
	void OptionsMenu::LayoutContentWidgets()
	{
		float y = m_contentsBeginY + m_scroll;
		size_t nextSectionTitle = 0;
		m_numSettingLabels = 0;
		
		const Font& titleFont = Font::GetNamedFont(FontNames::MenuTitle);
		
		auto AddUIElement = [&] (IUIElement& element, std::string label)
		{
			glm::vec2 elementSize = element.GetSize();
			
			const float ELEMENT_MARGIN = 10;
			
			m_settingLabels.at(m_numSettingLabels++) = 
			{
				.string = std::move(label),
				.rectangle = Rectangle(0, y - elementSize.y, m_contentsBeginX - 10, elementSize.y)
			};
			
			element.SetPosition(glm::vec2(m_contentsBeginX, y) + elementSize * glm::vec2(0.5f, -0.5f));
			y -= elementSize.y + ELEMENT_MARGIN;
		};
		
		auto AddSectionTitle = [&] (std::string title)
		{
			const float TITLE_MARGIN_UP = 30;
			const float TITLE_MARGIN_DOWN = 10;
			
			glm::vec2 textSize = titleFont.MeasureString(title);
			
			m_sectionTitleLabels.at(nextSectionTitle++) = 
			{
				.string = std::move(title),
				.rectangle = Rectangle(m_scissorArea.x, y - TITLE_MARGIN_UP - textSize.y, m_scissorArea.w, textSize.y)
			};
			
			y -= textSize.y + TITLE_MARGIN_UP + TITLE_MARGIN_DOWN;
		};
		
		AddSectionTitle("Display");
		if (m_displayModeComboBox) AddUIElement(*m_displayModeComboBox, "Display Mode");
		if (m_resolutionsComboBox) AddUIElement(*m_resolutionsComboBox, "Fullscreen Resolution");
		if (PLATFORM_CAN_CHANGE_VSYNC) AddUIElement(m_vSyncComboBox, "V-Sync");
		AddUIElement(m_gammaSlider, "Gamma"),
		AddSectionTitle("Graphics Quality");
		AddUIElement(m_resolutionScaleComboBox, "Resolution Scale");
		AddUIElement(m_lightingQualityComboBox, "Lighting");
		AddUIElement(m_particlesQualityComboBox, "Particles");
		AddUIElement(m_postQualityComboBox, "Post Processing");
		AddUIElement(m_bloomComboBox, "Bloom");
		AddSectionTitle("Sound");
		AddUIElement(m_masterVolumeSlider, "Master Volume");
		AddUIElement(m_musicVolumeSlider, "Music Volume");
		AddUIElement(m_sfxVolumeSlider, "Effects Volume");
	}
}
