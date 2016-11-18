#include "optionsmenu.h"
#include "../../updateinfo.h"
#include "../../graphics/ui/uirenderer.h"
#include "../../graphics/ui/font.h"
#include "../../graphics/scissor.h"
#include "../../settings.h"
#include "../../utils/utils.h"

#include <sstream>

namespace TankGame
{
	static ComboBox CreateQualityComboBox()
	{
		return ComboBox{ U"Low", U"Medium", U"High" };
	}
	
	const std::array<std::u32string, OptionsMenu::NUM_SECTIONS> OptionsMenu::SECTION_TITLES = 
	{
		U"Display",
		U"Graphics Quality",
		U"Sound"
	};
	
	OptionsMenu::OptionsMenu()
	    : m_settingLabels{
	          Label(U"Display Mode"), Label(U"Fullscreen Resolution"),
	          Label(U"Lighting"), Label(U"Particles"), Label(U"Post Processing"),
	          Label(U"Master Volume"), Label(U"Music Volume"), Label(U"SFX Volume")
	      },
	      m_backButton(U"Back"), m_applyButton(U"Apply"), m_displayModeComboBox{ U"Fullscreen", U"Windowed" },
	      m_lightingQualityComboBox(CreateQualityComboBox()), m_particlesQualityComboBox(CreateQualityComboBox()),
	      m_postQualityComboBox(CreateQualityComboBox()),
	      m_masterVolumeSlider(0, 100, 5), m_musicVolumeSlider(0, 100, 5), m_sfxVolumeSlider(0, 100, 5)
	{
		for (int i = 0; i < Settings::GetResolutionsCount(); i++)
		{
			std::ostringstream labelStream;
			
			glm::ivec2 res = Settings::GetResolution(i);
			labelStream << res.x << "x" << res.y;
			
			m_resolutionsComboBox.AddEntry(UTF8ToUTF32(labelStream.str()));
		}
		
		m_displayModeIndex = Settings::GetInstance().IsFullscreen() ? 0 : 1;
		
		m_currentResolutionIndex = Settings::GetDefaultResolutionIndex();
	}
	
	void OptionsMenu::OnResize(int newWidth, int newHeight)
	{
		m_backButton.SetPosition(glm::vec2(newWidth / 2.0f - 100, 100));
		m_applyButton.SetPosition(glm::vec2(newWidth / 2.0f + 100, 100));
		
		m_contentsBeginY = newHeight - 100;
		m_contentsBeginX = newWidth / 2.0f;
		
		m_scissorArea = { 0, 120, static_cast<float>(newWidth), m_contentsBeginY - 120 };
		
		LayoutContentWidgets();
	}
	
	void OptionsMenu::Update(const UpdateInfo& updateInfo)
	{
		if (m_backButton.Update(updateInfo) && m_backCallback != nullptr)
			m_backCallback();
		
		if (m_applyButton.Update(updateInfo) && m_applyCallback != nullptr)
		{
			glm::ivec2 res = Settings::GetResolution(m_currentResolutionIndex);
			m_applyCallback(m_displayModeIndex == 0, res.x, res.y);
		}
		
		bool anyDropDownShown = m_resolutionsComboBox.IsDropDownShown() ||
		        m_displayModeComboBox.IsDropDownShown() ||
		        m_lightingQualityComboBox.IsDropDownShown() ||
		        m_particlesQualityComboBox.IsDropDownShown() ||
		        m_postQualityComboBox.IsDropDownShown();
		
		if (!anyDropDownShown || m_resolutionsComboBox.IsDropDownShown())
		{
			m_resolutionsComboBox.Update(updateInfo, m_currentResolutionIndex);
		}
		
		if (!anyDropDownShown || m_displayModeComboBox.IsDropDownShown())
		{
			m_displayModeComboBox.Update(updateInfo, m_displayModeIndex);
		}
		
		if (!anyDropDownShown || m_lightingQualityComboBox.IsDropDownShown())
		{
			long lightingQuality = static_cast<long>(Settings::GetInstance().GetLightingQuality());
			if (m_lightingQualityComboBox.Update(updateInfo, lightingQuality))
				Settings::GetInstance().SetLightingQuality(static_cast<QualitySettings>(lightingQuality));
		}
		
		if (!anyDropDownShown || m_particlesQualityComboBox.IsDropDownShown())
		{
			long particlesQuality = static_cast<long>(Settings::GetInstance().GetParticleQuality());
			if (m_particlesQualityComboBox.Update(updateInfo, particlesQuality))
				Settings::GetInstance().SetParticleQuality(static_cast<QualitySettings>(particlesQuality));
		}
		
		if (!anyDropDownShown || m_postQualityComboBox.IsDropDownShown())
		{
			long postQuality = static_cast<long>(Settings::GetInstance().GetPostProcessingQuality());
			if (m_postQualityComboBox.Update(updateInfo, postQuality))
				Settings::GetInstance().SetPostProcessingQuality(static_cast<QualitySettings>(postQuality));
		}
		
		if (!anyDropDownShown)
		{
			float volume = Settings::GetInstance().GetMasterVolume() * 100.0f;
			if (m_masterVolumeSlider.Update(updateInfo, volume))
				Settings::GetInstance().SetMasterVolume(volume / 100.0f);
		}
		
		if (!anyDropDownShown)
		{
			float volume = Settings::GetInstance().GetMusicVolume() * 100.0f;
			if (m_musicVolumeSlider.Update(updateInfo, volume))
				Settings::GetInstance().SetMusicVolume(volume / 100.0f);
		}
		
		if (!anyDropDownShown)
		{
			float volume = Settings::GetInstance().GetSFXVolume() * 100.0f;
			if (m_sfxVolumeSlider.Update(updateInfo, volume))
				Settings::GetInstance().SetSFXVolume(volume / 100.0f);
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
		
		for (size_t i = 0; i < NUM_SECTIONS; i++)
		{
			uiRenderer.DrawString(Font::GetNamedFont(FontNames::MenuTitle), SECTION_TITLES[i],
			                      m_sectionTitleRectangles[i], Alignment::Center, Alignment::Center, glm::vec4(1.0f));
		}
		
		for (const Label& label : m_settingLabels)
		{
			uiRenderer.DrawString(Font::GetNamedFont(FontNames::StandardUI), label.m_string, label.m_rectangle,
			                      Alignment::Right, Alignment::Center, glm::vec4(1.0f));
		}
		
		//It is important to draw elements in reverse order!
		m_postQualityComboBox.Draw(uiRenderer);
		m_particlesQualityComboBox.Draw(uiRenderer);
		m_lightingQualityComboBox.Draw(uiRenderer);
		m_resolutionsComboBox.Draw(uiRenderer);
		m_displayModeComboBox.Draw(uiRenderer);
		m_masterVolumeSlider.Draw(uiRenderer);
		m_musicVolumeSlider.Draw(uiRenderer);
		m_sfxVolumeSlider.Draw(uiRenderer);
		
		PopScissorRect();
	}
	
	void OptionsMenu::LayoutContentWidgets()
	{
		//A nullptr means the title for that section
		IUIElement* uiElements[] = 
		{
			nullptr,
			&m_displayModeComboBox,
			&m_resolutionsComboBox,
			nullptr,
			&m_lightingQualityComboBox,
			&m_particlesQualityComboBox,
			&m_postQualityComboBox,
			nullptr,
			&m_masterVolumeSlider,
			&m_musicVolumeSlider,
			&m_sfxVolumeSlider
		};
		
		float y = m_contentsBeginY;
		int nextSectionTitle = 0;
		int nextSettingLabel = 0;
		
		const Font& titleFont = Font::GetNamedFont(FontNames::MenuTitle);
		
		for (IUIElement* uiElement : uiElements)
		{
			if (uiElement == nullptr)
			{
				const float TITLE_MARGIN_UP = 30;
				const float TITLE_MARGIN_DOWN = 10;
				
				glm::vec2 textSize = titleFont.MeasureString(SECTION_TITLES[nextSectionTitle]);
				
				m_sectionTitleRectangles[nextSectionTitle] = { m_scissorArea.x, y - TITLE_MARGIN_UP - textSize.y,
				                                               m_scissorArea.w, textSize.y };
				
				y -= textSize.y + TITLE_MARGIN_UP + TITLE_MARGIN_DOWN;
				
				nextSectionTitle++;
			}
			else
			{
				glm::vec2 elementSize = uiElement->GetSize();
				
				const float ELEMENT_MARGIN = 10;
				
				m_settingLabels[nextSettingLabel].m_rectangle = { 0, y - elementSize.y,
				                                                  m_contentsBeginX - 10, elementSize.y };
				
				uiElement->SetPosition(glm::vec2(m_contentsBeginX, y) + elementSize * glm::vec2(0.5f, -0.5f));
				y -= elementSize.y + ELEMENT_MARGIN;
				
				nextSettingLabel++;
			}
		}
	}
}
