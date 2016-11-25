#pragma once

#include <vector>
#include <functional>

#include "../rectangle.h"
#include "../levelmenuinfo.h"
#include "../utils/memory/stackobject.h"

namespace TankGame
{
	class LevelsList
	{
	public:
		using LoadLevelCallback = std::function<void(const std::string& name, int checkpoint)>;
		
		LevelsList();
		
		static void LoadLevelMenuInfos();
		
		void SetDrawRectangle(const Rectangle& rectangle);
		
		void Update(const class UpdateInfo& updateInfo);
		void Draw(const class UIRenderer& uiRenderer) const;
		
		inline void SetLoadLevelCallback(LoadLevelCallback callback)
		{ m_loadLevelCallback = std::move(callback); }
		
	private:
		struct ListLevelMenuInfo : public LevelMenuInfo
		{
		public:
			inline explicit ListLevelMenuInfo(std::istream& stream, std::string levelFileName)
			    : LevelMenuInfo(stream), m_levelFileName(std::move(levelFileName)) { }
			
			std::string m_levelFileName;
		};
		
		static std::vector<ListLevelMenuInfo> s_levelMenuInfos;
		
		static StackObject<Texture2D> s_playIcon;
		static StackObject<Texture2D> s_lockIcon;
		
		static constexpr float LEVEL_BOX_SPACING = 10;
		
		static constexpr float LEVEL_BOX_PADDING = 20;
		static constexpr float LEVEL_NAME_HEIGHT = 30;
		static constexpr float LEVEL_NAME_SPACING = 20;
		
		static constexpr float CHECKPOINT_IMAGE_HEIGHT = 100;
		static constexpr float CHECKPOINT_IMAGE_SPACING = 20;
		
		static constexpr float CHECKPOINT_LABEL_HEIGHT = 20;
		static constexpr float LEVEL_BOX_HEIGHT = LEVEL_NAME_HEIGHT + LEVEL_BOX_PADDING * 2 +
		        LEVEL_NAME_SPACING + CHECKPOINT_IMAGE_HEIGHT + CHECKPOINT_LABEL_HEIGHT;
		
		void UpdateLevelRectangles();
		
		LoadLevelCallback m_loadLevelCallback;
		
		struct StartLocation
		{
			bool m_unlocked = false;
			float m_hoverProgress = 0.0f;
			Rectangle m_rectangle;
			int m_checkpointIndex = -1;
			const Texture2D* m_image = nullptr;
		};
		
		struct LevelEntry
		{
			Rectangle m_rectangle;
			std::vector<StartLocation> m_startLocations;
			
			float m_scroll = 0.0f;
			float m_maxScroll;
			
			void SetScroll(float scroll);
		};
		
		std::vector<LevelEntry> m_levelEntries;
		
		Rectangle m_rectangle;
	};
}
