#pragma once

#include "../updateinfo.h"
#include "../graphics/viewinfo.h"
#include "../graphics/ui/uirenderer.h"
#include "../world/gameworld.h"
#include "../world/serialization/serializeworld.h"
#include "editorrenderer.h"
#include "tools/editortool.h"
#include "tools/entitytool.h"
#include "tools/tilestool.h"
#include "tools/pathedittool.h"
#include "saveasdialog.h"
#include "properties/propertieswindow.h"

namespace TankGame
{
	class Editor
	{
	public:
		explicit Editor(class GameManager& gameManager);
		
		inline ViewInfo GetViewInfo(float aspectRatio) const
		{ return { m_focusLocation, 0, m_viewWidth, aspectRatio }; }
		
		void OnResize(int newWidth, int newHeight);
		
		void Update(const UpdateInfo& updateInfo);
		
		[[nodiscard]] bool LoadLevel(std::string name);
		void Close();
		
		void StopTest()
		{ m_isTesting = false; }
		
		inline bool IsOpen() const
		{ return m_gameWorld != nullptr && !m_isTesting; }
		
		const EditorRenderer& GetRenderer();
		
		void DrawUI(const ViewInfo& viewInfo);
		
		void ShowPropertiesWindow(IPropertiesObject& object);
		
		inline IPropertiesObject* GetFocusedPropertiesObject() const
		{ return m_focusedPropertiesObject; }
		
		void EditPath(class IEditablePathProvider& provider);
		
		void UpdateShadows();
		
	private:
		void Save();
		void SaveAs();
		void TestLevel();
		
		glm::vec2 m_contextMenuWorldCoords;
		
		SaveAsDialog m_saveAsDialog;
		
		std::vector<PropertiesWindow> m_propertyWindows;
		
		std::unique_ptr<EditorRenderer> m_editorRenderer;
		
		IPropertiesObject* m_focusedPropertiesObject = nullptr;
		
		class GameManager& m_gameManager;
		
		EntityTool m_entityTool;
		TilesTool m_tilesTool;
		PathEditTool m_pathEditTool;
		
		bool m_isTesting = false;
		
		int m_currentToolIndex = 0;
		EditorTool* m_tools[3];
		
		glm::vec2 m_focusLocation;
		float m_viewWidth;
		
		glm::vec2 m_halfScreenSize;
		
		bool m_sendInputsToPropertyWindow = false;
		
		std::string m_currentLevelName;
		std::unique_ptr<GameWorld> m_gameWorld;
	};
}
