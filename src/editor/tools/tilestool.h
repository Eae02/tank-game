#pragma once

#include "editortool.h"
#include "../../graphics/gl/texture2d.h"
#include "../../graphics/gl/buffer.h"
#include "../../graphics/gl/vertexarray.h"
#include "../../graphics/gl/shaderprogram.h"
#include "../../utils/memory/stackobject.h"

#include <cstdint>

namespace TankGame
{
	class TilesTool : public EditorTool
	{
	public:
		explicit TilesTool(class Editor& editor);
		
		virtual void Update(const UpdateInfo& updateInfo) override;
		virtual void DrawUI(const class ViewInfo& viewInfo, class UIRenderer& uiRenderer) override;
		
		virtual void SetGameWorld(class GameWorld& gameWorld) override;
		
		virtual void RenderViewMenu() override;
		
	private:
		bool m_isMouseCaptured = false;
		
		bool m_drawGrid = true;
		
		struct TileMaterial
		{
			std::string m_name;
			uint8_t m_id;
			
			inline TileMaterial(std::string name, uint8_t id)
			    : m_name(std::move(name)), m_id(id) { }
		};
		
		std::vector<TileMaterial> m_tileMaterials;
		int m_currentMaterialIndex = 0;
		
		uint8_t m_currentTileID = 1;
		
		glm::ivec2 m_selectedTile;
		
		ShaderProgram m_gridShader;
		
		StackObject<Buffer> m_gridBuffer;
		GLsizei m_gridVertexCount;
		
		VertexArray m_gridVAO;
	};
}
