#include "tilestool.h"
#include "../editor.h"
#include "../../graphics/ui/uirenderer.h"
#include "../../graphics/tilegridmaterial.h"
#include "../../world/gameworld.h"
#include "../../graphics/gl/shadermodule.h"
#include "../../utils/ioutils.h"
#include "../../mouse.h"

#include <imgui.h>

namespace TankGame
{
	static ShaderProgram LoadGridShader()
	{
		return ShaderProgram(ShaderModule::FromResFile("editor/grid.vs.glsl"), ShaderModule::FromResFile("editor/grid.fs.glsl"));
	}
	
	TilesTool::TilesTool(class Editor& editor)
	    : EditorTool(editor), m_gridShader(LoadGridShader())
	{
		for (int i = 0; i < TileGridMaterial::GetInstance().GetMaterialCount() && i < 255; i++)
		{
			m_tileMaterials.emplace_back(TileGridMaterial::GetInstance().GetMaterialName(i), static_cast<uint8_t>(i));
		}
		
		std::sort(m_tileMaterials.begin(), m_tileMaterials.end(), [] (const TileMaterial& a, const TileMaterial& b)
		{ return std::less<std::string>()(a.m_name, b.m_name); });
		
		auto currentTilePos = std::find_if(m_tileMaterials.begin(), m_tileMaterials.end(), [this] (const auto& material)
		{
			return material.m_id == m_currentTileID;
		});
		
		assert(currentTilePos != m_tileMaterials.end());
		
		m_currentMaterialIndex = currentTilePos - m_tileMaterials.begin();
	}
	
	void TilesTool::Update(const UpdateInfo& updateInfo)
	{
		m_isMouseCaptured = updateInfo.m_mouse.isCaptured;
		
		TileGrid* grid = GetGameWorld().GetTileGrid();
		
		glm::vec2 worldMouseCoords = GetNewWorldMouseCoords(updateInfo);
		m_selectedTile = { std::floor(worldMouseCoords.x), std::floor(worldMouseCoords.y) };
		if (m_selectedTile.x < 0 || m_selectedTile.y < 0 || m_selectedTile.x >= grid->GetWidth() ||
		    m_selectedTile.y >= grid->GetHeight())
		{
			m_selectedTile = { -1, -1 };
		}
		else
		{
			if (updateInfo.m_mouse.IsDown(MouseButton::Left))
			{
				if (grid->GetTileID(m_selectedTile) != m_currentTileID)
				{
					grid->SetTileID(m_selectedTile, m_currentTileID);
					grid->UploadGridData(m_selectedTile.x, m_selectedTile.y, 1, 1);
					m_tileGridChanged = true;
				}
			}
			else if (updateInfo.m_mouse.WasDown(MouseButton::Left) && m_tileGridChanged)
			{
				GetEditor().UpdateShadows();
				m_tileGridChanged = false;
			}
		}
	}
	
	void TilesTool::DrawUI(const ViewInfo& viewInfo, struct UIRenderer& uiRenderer)
	{
		if (m_drawGrid)
		{
			m_gridShader.Use();
			m_gridVertexInputState.Bind();
			glDrawArrays(GL_LINES, 0, m_gridVertexCount);
		}
		
		const glm::vec4 HIGHLIGHT_COLOR = { 0.8f, 0.8f, 0.8f, 0.3f };
		
		if (m_selectedTile.x != -1 && !m_isMouseCaptured)
		{
			glm::vec2 screenSize(GetWindowWidth(), GetWindowHeight());
			glm::vec2 min = viewInfo.WorldToScreen(m_selectedTile) * screenSize;
			glm::vec2 max = viewInfo.WorldToScreen(m_selectedTile + glm::ivec2(1)) * screenSize;
			
			uiRenderer.DrawRectangle(Rectangle::FromMinMax(min, max), HIGHLIGHT_COLOR);
		}
		
		if (ImGui::Begin("Tiles"))
		{
			ImGui::PushItemWidth(-1);
			
			bool changed = ImGui::ListBox("##Tiles", &m_currentMaterialIndex, [] (void* data, int i, const char** out)
			{
				*out = reinterpret_cast<TilesTool*>(data)->m_tileMaterials[i].m_name.c_str();
				return true;
			}, this, m_tileMaterials.size());
			
			ImGui::PopItemWidth();
			
			if (changed)
				m_currentTileID = m_tileMaterials[m_currentMaterialIndex].m_id;
		}
		
		ImGui::End();
	}
	
	struct GridLine
	{
		glm::vec2 begin;
		glm::vec2 end;
		
		inline GridLine(const glm::vec2& begin, const glm::vec2& end)
		    : begin(begin), end(end) { }
	};
	
	void TilesTool::SetGameWorld(class GameWorld& gameWorld)
	{
		TileGrid* grid = gameWorld.GetTileGrid();
		
		std::vector<GridLine> bufferData;
		bufferData.reserve(grid->GetWidth() + grid->GetHeight() + 2);
		
		for (int i = 0; i <= grid->GetWidth(); i++)
			bufferData.emplace_back(glm::vec2(i, 0), glm::vec2(i, grid->GetHeight()));
		for (int i = 0; i <= grid->GetHeight(); i++)
			bufferData.emplace_back(glm::vec2(0, i), glm::vec2(grid->GetWidth(), i));
		
		m_gridVertexCount = bufferData.size() * 2;
		
		m_gridBuffer = std::make_unique<Buffer>(bufferData.size() * sizeof(GridLine), bufferData.data(), BufferUsage::StaticVertex);
		
		m_gridVertexInputState.UpdateAttribute(0, m_gridBuffer->GetID(), VertexAttribFormat::Float32_2, 0, sizeof(float) * 2);
		
		EditorTool::SetGameWorld(gameWorld);
	}
	
	void TilesTool::RenderViewMenu()
	{
		ImGui::Separator();
		
		ImGui::MenuItem("Tile Grid", nullptr, &m_drawGrid);
	}
}
