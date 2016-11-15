#include "hudmanager.h"
#include "../utils/ioutils.h"
#include "../utils/utils.h"
#include "../world/entities/playerentity.h"
#include "../graphics/quadmesh.h"
#include "../graphics/gl/shadermodule.h"
#include "../graphics/ui/font.h"
#include "../graphics/imainrenderer.h"
#include "../updateinfo.h"

#include <glm/gtc/constants.hpp>

namespace TankGame
{
#define PERCENT / 100.0f
	
	const float PADDING = 2 PERCENT;
	
	const float HP_BAR_WIDTH = 20 PERCENT;
	const float CHECKPOINT_REACHED_WIDTH = 40 PERCENT;
	
	static ShaderProgram LoadShader()
	{
		fs::path shaderPath = GetResDirectory() / "shaders" / "ui";
		auto vs = ShaderModule::FromFile(shaderPath / "hud.vs.glsl", GL_VERTEX_SHADER);
		auto fs = ShaderModule::FromFile(shaderPath / "hud.fs.glsl", GL_FRAGMENT_SHADER);
		return ShaderProgram({ &vs, &fs });
	}
	
	static Buffer CreateVertexBuffer(GLsizei& vertexCountOut)
	{
		const int COLUMNS = 30;
		
		struct ColumnVertices
		{
			glm::vec3 m_bottomVertexPos;
			glm::vec2 m_bottomVertexTC;
			
			glm::vec3 m_topVertexPos;
			glm::vec2 m_topVertexTC;
		};
		
		ColumnVertices vertices[COLUMNS + 1];
		
		for (int i = 0; i < COLUMNS + 1; i++)
		{
			float theta = glm::mix(glm::radians(20.0f), glm::radians(160.0f), i / static_cast<float>(COLUMNS));
			
			float x = std::cos(theta);
			float z = glm::mix(1.0f, 1.2f, std::sin(theta));
			
			vertices[i].m_topVertexTC.x = vertices[i].m_bottomVertexTC.x = 1.0f - i / static_cast<float>(COLUMNS);
			vertices[i].m_topVertexTC.y = 1;
			vertices[i].m_bottomVertexTC.y = 0;
			
			vertices[i].m_topVertexPos.x = vertices[i].m_bottomVertexPos.x = x;
			vertices[i].m_topVertexPos.z = vertices[i].m_bottomVertexPos.z = z;
			
			vertices[i].m_bottomVertexPos.y = -1;
			vertices[i].m_topVertexPos.y = 1;
		}
		
		vertexCountOut = (COLUMNS + 1) * 2;
		
		return Buffer(sizeof(vertices), vertices, 0);
	}
	
	HUDManager::HUDManager(IMainRenderer& mainRenderer)
	    : m_mainRenderer(mainRenderer),
	      m_vertexBuffer(CreateVertexBuffer(m_vertexCount)),
	      m_textures(GetResDirectory() / "ui"), m_shader(LoadShader())
	{
		int attribSizes[] = { 3, 2 };
		int offset = 0;
		for (GLuint i = 0; i < ArrayLength(attribSizes); i++)
		{
			glEnableVertexArrayAttrib(m_vertexArray.GetID(), i);
			glVertexArrayVertexBuffer(m_vertexArray.GetID(), i, m_vertexBuffer.GetID(),
			                          sizeof(float) * offset, sizeof(float) * 5);
			glVertexArrayAttribFormat(m_vertexArray.GetID(), i, attribSizes[i], GL_FLOAT, GL_FALSE, 0);
			glVertexArrayAttribBinding(m_vertexArray.GetID(), i, i);
			
			offset += attribSizes[i];
		}
	}
	
	void HUDManager::OnResize(GLsizei width, GLsizei height)
	{
		if (!m_fbTexture.IsNull() && m_fbTexture->GetWidth() == width && m_fbTexture->GetHeight() == height)
			return;
		
		m_fbTexture.Construct(width, height, 1, GL_RGBA8);
		m_fbTexture->SetWrapMode(GL_CLAMP_TO_EDGE);
		
		m_framebuffer.Construct();
		glNamedFramebufferTexture(m_framebuffer->GetID(), GL_COLOR_ATTACHMENT0, m_fbTexture->GetID(), 0);
		glNamedFramebufferDrawBuffer(m_framebuffer->GetID(), GL_COLOR_ATTACHMENT0);
		
		m_uiRenderer.SetWindowDimensions(width, height);
		
		LayoutElements(width, height);
		
		m_pauseMenu.OnResize(width, height);
	}
	
	void HUDManager::Update(const UpdateInfo& updateInfo)
	{
		if (m_playerEntity != nullptr)
		{
			float deltaHp = m_playerEntity->GetHp() - m_hp;
			m_hp += deltaHp * glm::min(updateInfo.m_dt * 5, 1.0f);
		}
		
		float blurAmountTarget = m_pauseMenu.IsShown() ? 1.0f : 0.0f;
		m_blurAmount = glm::clamp(m_blurAmount + glm::sign(blurAmountTarget - m_blurAmount) * updateInfo.m_dt * 5, 0.0f, 1.0f);
		m_mainRenderer.SetBlurAmount(m_blurAmount);
		
		m_pauseMenu.Update(updateInfo);
	}
	
	void HUDManager::LayoutElements(GLsizei screenWidth, GLsizei screenHeight)
	{
		float padding = PADDING * screenWidth;
		
		Rectangle contentsRect(padding, padding, screenWidth - 2 * padding, screenHeight - 2 * padding);
		
		float hpBarAspectRatio = m_textures.m_hpBarFull.GetHeight() / static_cast<float>(m_textures.m_hpBarFull.GetWidth());
		
		m_hpBarRectangle.x = contentsRect.x;
		m_hpBarRectangle.y = contentsRect.y;
		m_hpBarRectangle.w = HP_BAR_WIDTH * screenWidth;
		m_hpBarRectangle.h = hpBarAspectRatio * m_hpBarRectangle.w;
	}
	
	void HUDManager::DrawHUD()
	{
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
		
		Framebuffer::Save();
		Framebuffer::Bind(*m_framebuffer, 0, 0, m_fbTexture->GetWidth(), m_fbTexture->GetHeight());
		
		float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glClearNamedFramebufferfv(m_framebuffer->GetID(), GL_COLOR, 0, clearColor);
		
		DrawHUDElements();
		
		Framebuffer::Restore();
		
		m_shader.Use();
		m_fbTexture->Bind(0);
		
		m_vertexArray.Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexCount);
		
		m_pauseMenu.Draw(m_uiRenderer);
		
		glDisable(GL_BLEND);
	}
	
	void HUDManager::SetPlayerEntity(const PlayerEntity* playerEntity)
	{
		m_playerEntity = playerEntity;
		m_hp = playerEntity->GetHp();
	}
	
	void HUDManager::DrawHUDElements()
	{
		const int FULL_HP_BAR_MARGIN_L = 29;
		const int FULL_HP_BAR_MARGIN_R = 37;
		int fullHpBarWidth = m_textures.m_hpBarFull.GetWidth() - FULL_HP_BAR_MARGIN_L - FULL_HP_BAR_MARGIN_R;
		
		float hpMul = m_hp / m_playerEntity->GetMaxHp();
		
		Rectangle fullHpBarSampleRect(0, 0, FULL_HP_BAR_MARGIN_L + fullHpBarWidth * hpMul,
		                              m_textures.m_hpBarFull.GetHeight());
		Rectangle emptyHpBarSampleRect(fullHpBarSampleRect.w, 0, m_textures.m_hpBarEmpty.GetWidth() -
		                               fullHpBarSampleRect.w, m_textures.m_hpBarFull.GetHeight());
		
		Rectangle fullHpBarTargetRect(m_hpBarRectangle.x, m_hpBarRectangle.y, m_hpBarRectangle.w * hpMul, m_hpBarRectangle.h);
		Rectangle emptyHpBarTargetRect(fullHpBarTargetRect.FarX(), m_hpBarRectangle.y, m_hpBarRectangle.w - fullHpBarTargetRect.w, m_hpBarRectangle.h);
		
		if (emptyHpBarSampleRect.w > 1E-6)
			m_uiRenderer.DrawSprite(m_textures.m_hpBarEmpty, emptyHpBarTargetRect, emptyHpBarSampleRect, glm::vec4(1.0f));
		if (fullHpBarSampleRect.w > 1E-6)
			m_uiRenderer.DrawSprite(m_textures.m_hpBarFull, fullHpBarTargetRect, fullHpBarSampleRect, glm::vec4(1.0f));
		
		std::string hpString = std::to_string(static_cast<int>(std::round(m_hp)));
		m_uiRenderer.DrawString(Font::GetNamedFont(FontNames::HudFont), hpString, m_hpBarRectangle,
		                        Alignment::Left, Alignment::Center, glm::vec4(1.0f));
	}
	
	HUDManager::Textures::Textures(const fs::path& dirPath)
	    : m_hpBarFull(Texture2D::FromFile(dirPath / "hp-full.png")),
	      m_hpBarEmpty(Texture2D::FromFile(dirPath / "hp-empty.png"))
	{
		
	}
}
