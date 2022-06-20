#include "levelslist.h"
#include "elements/iuielement.h"
#include "../level.h"
#include "../updateinfo.h"
#include "../mouse.h"
#include "../graphics/scissor.h"
#include "../graphics/ui/uirenderer.h"
#include "../graphics/ui/font.h"
#include "../utils/ioutils.h"
#include "../progress.h"

#include <fstream>

namespace TankGame
{
	std::vector<LevelsList::ListLevelMenuInfo> LevelsList::s_levelMenuInfos;
	std::unique_ptr<Texture2D> LevelsList::s_playIcon;
	std::unique_ptr<Texture2D> LevelsList::s_lockIcon;
	
	LevelsList::LevelsList() : m_levelEntries(s_levelMenuInfos.size())
	{
		for (size_t i = 0; i < s_levelMenuInfos.size(); i++)
		{
			m_levelEntries[i].m_startLocations.resize(s_levelMenuInfos[i].GetCheckpointCount() + 1, StartLocation());
			
			m_levelEntries[i].m_startLocations[0].m_image = &s_levelMenuInfos[i].GetStartImage();
			
			for (size_t c = 0; c < s_levelMenuInfos[i].GetCheckpointCount(); c++)
			{
				m_levelEntries[i].m_startLocations[c + 1].m_image = &s_levelMenuInfos[i].GetCheckpointImage(c);
				m_levelEntries[i].m_startLocations[c + 1].m_checkpointIndex = s_levelMenuInfos[i].GetCheckpointIndex(c);
			}
		}
		
		if (s_playIcon== nullptr)
		{
			s_playIcon = std::make_unique<Texture2D>(Texture2D::FromFile(resDirectoryPath / "ui" / "play.png"));
			CallOnClose([] { s_playIcon = nullptr; });
		}
		
		if (s_lockIcon== nullptr)
		{
			s_lockIcon = std::make_unique<Texture2D>(Texture2D::FromFile(resDirectoryPath / "ui" / "lock.png"));
			CallOnClose([] { s_lockIcon = nullptr; });
		}
	}
	
	void LevelsList::LoadLevelMenuInfos()
	{
		s_levelMenuInfos.clear();
		
		for (const fs::directory_entry& entry : fs::directory_iterator(Level::GetLevelsPath()))
		{
			fs::path miPath = entry.path();
			miPath += ".mi";
			
			if (fs::exists(miPath))
			{
				std::ifstream stream(miPath, std::ios::binary);
				s_levelMenuInfos.emplace_back(stream, entry.path().filename().string());
			}
		}
		
		std::sort(s_levelMenuInfos.begin(), s_levelMenuInfos.end());
	}
	
	void LevelsList::SetDrawRectangle(const Rectangle& rectangle)
	{
		m_rectangle = rectangle;
		
		for (LevelEntry& levelEntry : m_levelEntries)
		{
			float w = -CHECKPOINT_IMAGE_SPACING;
			
			for (const StartLocation& sl : levelEntry.m_startLocations)
			{
				float imageAR = sl.m_image->GetWidth() / static_cast<float>(sl.m_image->GetHeight());
				w += CHECKPOINT_IMAGE_HEIGHT * imageAR + CHECKPOINT_IMAGE_SPACING;
			}
			
			levelEntry.m_maxScroll = glm::max(w - (rectangle.w - LEVEL_BOX_PADDING * 2), 0.0f);
			levelEntry.SetScroll(levelEntry.m_scroll);
		}
		
		UpdateLevelRectangles();
	}
	
	void LevelsList::Update(const UpdateInfo& updateInfo)
	{
		for (size_t i = 0; i < s_levelMenuInfos.size(); i++)
		{
			int progress = Progress::GetInstance().GetLevelProgress(s_levelMenuInfos[i].m_levelFileName);
			
			if (m_levelEntries[i].m_rectangle.Contains(updateInfo.m_mouse.pos) &&
			    std::abs(updateInfo.m_mouse.GetDeltaScroll()) > 1E-6f)
			{
				m_levelEntries[i].SetScroll(m_levelEntries[i].m_scroll - updateInfo.m_mouse.GetDeltaScroll() * 30);
				UpdateLevelRectangles();
			}
			
			for (StartLocation& startLocation : m_levelEntries[i].m_startLocations)
			{
				startLocation.m_unlocked = progress >= startLocation.m_checkpointIndex;
				
				float targetHoverProgress = 0.0f;
				if (startLocation.m_unlocked && startLocation.m_rectangle.Contains(updateInfo.m_mouse.pos))
				{
					targetHoverProgress = 1.0f;
					
					if (updateInfo.m_mouse.IsDown(MouseButton::Left))
						targetHoverProgress = 0.9f;
					else if (updateInfo.m_mouse.WasDown(MouseButton::Left) && m_loadLevelCallback)
						m_loadLevelCallback(s_levelMenuInfos[i].m_levelFileName, startLocation.m_checkpointIndex);
					
					if (startLocation.m_hoverProgress == 0.0f)
						IUIElement::PlayMouseOverEffect();
				}
				
				UpdateTransition(startLocation.m_hoverProgress, targetHoverProgress,
				                 updateInfo.m_dt * 10.0f);
			}
		}
	}
	
	void LevelsList::Draw(const UIRenderer& uiRenderer) const
	{
		PushScissorRect(m_rectangle);
		
		for (size_t i = 0; i < s_levelMenuInfos.size(); i++)
		{
			//Draws the level background
			uiRenderer.DrawRectangle(m_levelEntries[i].m_rectangle, glm::vec4(ParseColorHexCodeSRGB(0x242424), 0.6f));
			
			Rectangle scissorRectangle = m_levelEntries[i].m_rectangle;
			scissorRectangle.Inflate(-LEVEL_BOX_PADDING);
			PushScissorRect({ scissorRectangle.x, m_rectangle.y, scissorRectangle.w, m_rectangle.h });
			
			//Draws the level name label
			Rectangle nameRectangle(m_levelEntries[i].m_rectangle.x + LEVEL_BOX_PADDING,
			                        m_levelEntries[i].m_rectangle.FarY() - (LEVEL_BOX_PADDING + LEVEL_NAME_HEIGHT),
			                        m_levelEntries[i].m_rectangle.w - LEVEL_BOX_PADDING * 2, LEVEL_NAME_HEIGHT);
			
			uiRenderer.DrawString(Font::GetNamedFont(FontNames::ButtonFont), s_levelMenuInfos[i].GetName(),
			                      nameRectangle, Alignment::Left, Alignment::Center, glm::vec4(1.0f), 0.7f);
			
			//Draws start locations
			for (size_t c = 0; c < m_levelEntries[i].m_startLocations.size(); c++)
			{
				Rectangle imageRect = m_levelEntries[i].m_startLocations[c].m_rectangle;
				
				//Draws the start location's image
				float shade = m_levelEntries[i].m_startLocations[c].m_unlocked ? 1.0f : 0.2f;
				uiRenderer.DrawSprite(*m_levelEntries[i].m_startLocations[c].m_image,
				                      imageRect, glm::vec4(shade, shade, shade, 1));
				
				//Draws the lock or play icons
				if (!m_levelEntries[i].m_startLocations[c].m_unlocked)
				{
					Rectangle lockIconRect = Rectangle::CreateCentered(imageRect.Center(), 50, 50);
					uiRenderer.DrawSprite(*s_lockIcon, lockIconRect, glm::vec4(1, 1, 1, 0.75f));
				}
				else if (m_levelEntries[i].m_startLocations[c].m_hoverProgress > 0)
				{
					float iconFade = m_levelEntries[i].m_startLocations[c].m_hoverProgress;
					
					float iconSize = glm::mix(50, 64, iconFade);
					Rectangle playIconRect = Rectangle::CreateCentered(imageRect.Center(), iconSize, iconSize);
					
					uiRenderer.DrawSprite(*s_playIcon, playIconRect, glm::vec4(1, 1, 1, iconFade * 0.75f));
				}
				
				//Draws the checkpoint label
				std::string label = c == 0 ? "Start" : "Checkpoint " + std::to_string(c);
				Rectangle labelRect(imageRect.x, imageRect.y - CHECKPOINT_LABEL_HEIGHT, imageRect.w, CHECKPOINT_LABEL_HEIGHT);
				uiRenderer.DrawString(Font::GetNamedFont(FontNames::StandardUI), label, labelRect,
				                      Alignment::Center, Alignment::Center, glm::vec4(1.0f));
			}
			
			PopScissorRect();
		}
		
		PopScissorRect();
	}
	
	void LevelsList::UpdateLevelRectangles()
	{
		float h = (LEVEL_BOX_HEIGHT + LEVEL_BOX_SPACING) * s_levelMenuInfos.size() - LEVEL_BOX_SPACING;
		
		float y = m_rectangle.CenterY() + h / 2.0f;
		for (size_t i = 0; i < s_levelMenuInfos.size(); i++)
		{
			y -= LEVEL_BOX_HEIGHT;
			
			m_levelEntries[i].m_rectangle.x = m_rectangle.x;
			m_levelEntries[i].m_rectangle.y = y;
			m_levelEntries[i].m_rectangle.w = m_rectangle.w;
			m_levelEntries[i].m_rectangle.h = LEVEL_BOX_HEIGHT;
			
			float checkpointX = m_rectangle.x + LEVEL_BOX_PADDING - m_levelEntries[i].m_scroll;
			for (size_t c = 0; c < m_levelEntries[i].m_startLocations.size(); c++)
			{
				const Texture2D& image = *m_levelEntries[i].m_startLocations[c].m_image;
				
				float width = CHECKPOINT_IMAGE_HEIGHT * (image.GetWidth() / static_cast<float>(image.GetHeight()));
				
				m_levelEntries[i].m_startLocations[c].m_rectangle =
				        Rectangle(checkpointX, m_levelEntries[i].m_rectangle.y + LEVEL_BOX_PADDING +
				                  CHECKPOINT_LABEL_HEIGHT, width, CHECKPOINT_IMAGE_HEIGHT);
				
				checkpointX += width + CHECKPOINT_IMAGE_SPACING;
			}
			
			y -= LEVEL_BOX_SPACING;
		}
	}
	
	void LevelsList::LevelEntry::SetScroll(float scroll)
	{
		m_scroll = glm::clamp(scroll, 0.0f, m_maxScroll);
	}
	
	
	
}
