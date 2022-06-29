#include "hudmanager.h"
#include "../utils/ioutils.h"
#include "../utils/utils.h"
#include "../world/entities/playerentity.h"
#include "../graphics/quadmesh.h"
#include "../graphics/gl/shadermodule.h"
#include "../graphics/ui/font.h"
#include "../graphics/imainrenderer.h"
#include "../updateinfo.h"
#include "../profiling.h"


namespace TankGame
{
#define PERCENT / 100.0f
	
	const float PADDING = 2 PERCENT;
	
	const float GLOBAL_HP_BAR_TOP_DIST = -5 PERCENT;
	const float GLOBAL_HP_BAR_WIDTH = 50 PERCENT;
	
	const float HP_BAR_WIDTH = 20 PERCENT;
	const float CHECKPOINT_REACHED_WIDTH = 40 PERCENT;
	
	const float WEAPON_ICON_WIDTH = 4 PERCENT;
	const float WEAPON_ICON_PADDING = 1 PERCENT;
	
	static ShaderProgram LoadShader()
	{
		fs::path shaderPath = resDirectoryPath / "shaders" / "ui";
		auto vs = ShaderModule::FromFile(shaderPath / "hud.vs.glsl", GL_VERTEX_SHADER);
		auto fs = ShaderModule::FromFile(shaderPath / "hud.fs.glsl", GL_FRAGMENT_SHADER);
		ShaderProgram program(vs, fs);
		program.SetTextureBinding("inputSampler", 0);
		return program;
	}
	
	struct ColumnVertex
	{
		glm::vec3 pos;
		glm::vec2 texCoord;
	};
	
	static Buffer CreateVertexBuffer(GLsizei& vertexCountOut)
	{
		const int COLUMNS = 30;
		
		ColumnVertex vertices[COLUMNS * 2 + 2];
		
		for (int i = 0; i <= COLUMNS; i++)
		{
			float theta = glm::mix(glm::radians(20.0f), glm::radians(160.0f), i / static_cast<float>(COLUMNS));
			
			float x = std::cos(theta);
			float z = glm::mix(1.0f, 1.2f, std::sin(theta));
			
			ColumnVertex& btmVertex = vertices[i * 2 + 0];
			ColumnVertex& topVertex = vertices[i * 2 + 1];
			
			float tcx = 1.0f - i / static_cast<float>(COLUMNS);
			topVertex.texCoord = glm::vec2(tcx, 1);
			btmVertex.texCoord = glm::vec2(tcx, 0);
			
			topVertex.pos = glm::vec3(x, 1, z);
			btmVertex.pos = glm::vec3(x, -1, z);
		}
		
		vertexCountOut = (COLUMNS + 1) * 2;
		
		return Buffer(sizeof(vertices), vertices, BufferUsage::StaticVertex);
	}
	
	HUDManager::HUDManager(IMainRenderer& mainRenderer)
	    : m_mainRenderer(mainRenderer),
	      m_weaponIcons{ WeaponIcon(0), WeaponIcon(1) },
	      m_vertexBuffer(CreateVertexBuffer(m_vertexCount)),
	      m_textures(resDirectoryPath / "ui"), m_shader(LoadShader())
	{
		WeaponIcon::MaybeLoadIcons();
		
		m_vertexInputState.UpdateAttribute(0, m_vertexBuffer.GetID(),
			VertexAttribFormat::Float32_3, offsetof(ColumnVertex, pos), sizeof(ColumnVertex));
		m_vertexInputState.UpdateAttribute(1, m_vertexBuffer.GetID(),
			VertexAttribFormat::Float32_2, offsetof(ColumnVertex, texCoord), sizeof(ColumnVertex));
	}
	
	void HUDManager::OnResize(GLsizei width, GLsizei height)
	{
		if (m_fbTexture != nullptr && m_fbTexture->GetWidth() == width && m_fbTexture->GetHeight() == height)
			return;
		
		m_fbTexture = std::make_unique<Texture2D>(width, height, 1, TextureFormat::RGBA8);
		m_fbTexture->SetWrapMode(GL_CLAMP_TO_EDGE);
		
		m_framebuffer = std::make_unique<Framebuffer>();
		glNamedFramebufferTexture(m_framebuffer->GetID(), GL_COLOR_ATTACHMENT0, m_fbTexture->GetID(), 0);
		glNamedFramebufferDrawBuffer(m_framebuffer->GetID(), GL_COLOR_ATTACHMENT0);
		
		LayoutElements(width, height);
		
		m_pauseMenu.OnResize(width, height);
		m_levelCompleteMenu.OnResize(width, height);
	}
	
	void HUDManager::Update(const UpdateInfo& updateInfo)
	{
		FUNC_TIMER
		
		if (m_playerEntity != nullptr)
		{
			float deltaHp = m_playerEntity->GetHp() - m_hp;
			m_hp += deltaHp * glm::min(updateInfo.m_dt * 5, 1.0f);
			
			float deltaEnergy = m_playerEntity->GetEnergy() - m_energy;
			m_energy += deltaEnergy * glm::min(updateInfo.m_dt * 10, 1.0f);
		}
		
		float deltaGlobalHP = m_globalHealthBarTargetPercentage - m_globalHealthBarPercentage;
		m_globalHealthBarPercentage += deltaGlobalHP * glm::min(updateInfo.m_dt * 5.0f, 0.1f);
		
		const float globalHealthBarOpacityDelta = (m_globalHealthBarVisible ? 1 : -1) * updateInfo.m_dt * 1.5f;
		m_globalHealthBarOpacity = glm::clamp(m_globalHealthBarOpacity + globalHealthBarOpacityDelta, 0.0f, 1.0f);
		
		float blurAmountTarget = IsPaused() ? 1.0f : 0.0f;
		m_blurAmount = glm::clamp(m_blurAmount + glm::sign(blurAmountTarget - m_blurAmount) * updateInfo.m_dt * 5, 0.0f, 1.0f);
		m_mainRenderer.SetBlurAmount(glm::smoothstep(0.0f, 1.0f, m_blurAmount));
		
		if (m_noAmmoOpacity > 0)
			m_noAmmoOpacity = glm::max(m_noAmmoOpacity - updateInfo.m_dt, 0.0f);
		
		if (m_levelCompleteMenu.IsShown())
			m_levelCompleteMenu.Update(updateInfo);
		else
			m_pauseMenu.Update(updateInfo);
		
		for (WeaponIcon& weaponIcon : m_weaponIcons)
			weaponIcon.Update(updateInfo);
	}
	
	void HUDManager::LayoutElements(GLsizei screenWidth, GLsizei screenHeight)
	{
		float padding = PADDING * screenWidth;
		
		m_contentsRect = Rectangle(padding, padding, screenWidth - 2 * padding, screenHeight - 2 * padding);
		
		float globalHpBarAR = m_textures.m_hpBarGlobalFull.GetHeight() / static_cast<float>(m_textures.m_hpBarGlobalFull.GetWidth());
		
		float hpBarAR = m_textures.m_hpBarFull.GetHeight() / static_cast<float>(m_textures.m_hpBarFull.GetWidth());
		float energyBarAR = m_textures.m_energyBarFull.GetHeight() / static_cast<float>(m_textures.m_energyBarFull.GetWidth());
		
		m_globalHpBarRectangle.x = m_contentsRect.x + m_contentsRect.w * (1.0f - GLOBAL_HP_BAR_WIDTH) * 0.5f;
		m_globalHpBarRectangle.w = m_contentsRect.w * GLOBAL_HP_BAR_WIDTH;
		m_globalHpBarRectangle.h = globalHpBarAR * m_globalHpBarRectangle.w;
		m_globalHpBarRectangle.y = m_contentsRect.FarY() - m_globalHpBarRectangle.h - GLOBAL_HP_BAR_TOP_DIST * m_contentsRect.h;
		
		m_hpBarRectangle.x = m_contentsRect.x;
		m_hpBarRectangle.y = m_contentsRect.y;
		m_hpBarRectangle.w = HP_BAR_WIDTH * screenWidth;
		m_hpBarRectangle.h = hpBarAR * m_hpBarRectangle.w;
		
		m_energyBarRectangle.x = m_contentsRect.x;
		m_energyBarRectangle.y = m_hpBarRectangle.FarY();
		m_energyBarRectangle.w = m_hpBarRectangle.w * (m_textures.m_energyBarFull.GetWidth() /
		                                               static_cast<float>(m_textures.m_hpBarFull.GetWidth()));
		m_energyBarRectangle.h = energyBarAR * m_energyBarRectangle.w;
		
		float weaponIconW = WEAPON_ICON_WIDTH * screenWidth;
		float weaponIconH = WEAPON_ICON_WIDTH * screenWidth;
		float weaponIconPadding = WEAPON_ICON_PADDING * screenWidth;
		
		m_weaponInfoRectangle = { m_contentsRect.x, m_contentsRect.y, m_contentsRect.w,
		                          static_cast<float>(Font::GetNamedFont(FontNames::HudFont).GetSize()) };
		
		float weaponIconsW = weaponIconW * m_weaponIcons.size() + weaponIconPadding * (m_weaponIcons.size() - 1);
		m_weaponIconsLineRectangle = Rectangle::CreateCentered(m_contentsRect.CenterX(), m_weaponInfoRectangle.FarY(),
		                                                       weaponIconsW * 1.5f, 1);
		
		float weaponIconX = m_contentsRect.CenterX() - weaponIconsW / 2.0f;
		for (size_t i = 0; i < m_weaponIcons.size(); i++)
		{
			m_weaponIcons[i].SetRectangle({ weaponIconX, m_weaponInfoRectangle.FarY(), weaponIconW, weaponIconH });
			weaponIconX += weaponIconW + weaponIconPadding;
		}
		
		m_noAmmoRectangle = Rectangle::CreateCentered(m_contentsRect.CenterX(), m_weaponInfoRectangle.FarY() + weaponIconH + 40,
		                                              m_textures.m_noAmmo.GetWidth(), m_textures.m_noAmmo.GetHeight());
	}
	
	void HUDManager::DrawHUD()
	{
		Framebuffer::Save();
		Framebuffer::Bind(*m_framebuffer, 0, 0, m_fbTexture->GetWidth(), m_fbTexture->GetHeight());
		
		float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glClearNamedFramebufferfv(m_framebuffer->GetID(), GL_COLOR, 0, clearColor);
		
		DrawHUDElements();
		
		Framebuffer::Restore();
		
		m_shader.Use();
		m_fbTexture->Bind(0);
		
		m_vertexInputState.Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexCount);
		
		if (m_levelCompleteMenu.IsShown())
			m_levelCompleteMenu.Draw(UIRenderer::GetInstance());
		else
			m_pauseMenu.Draw(UIRenderer::GetInstance());
	}
	
	void HUDManager::SetPlayerEntity(const PlayerEntity* playerEntity)
	{
		m_playerEntity = playerEntity;
		
		if (playerEntity != nullptr)
		{
			m_hp = playerEntity->GetHp();
			m_energy = playerEntity->GetEnergy();
		}
		
		m_noAmmoOpacity = 0;
		
		for (WeaponIcon& weaponIcon : m_weaponIcons)
			weaponIcon.SetWeaponState(playerEntity == nullptr ? nullptr : &playerEntity->GetWeaponState());
	}
	
	int HUDManager::GetWeaponIndex(const PlayerWeaponState& weaponState)
	{
		if (weaponState.IsUsingPlasmaGun())
			return 0;
		return static_cast<int>(weaponState.GetCurrentSpecialWeapon()) + 1;
	}
	
	void HUDManager::DrawHUDElements()
	{
		const Font& hudFont = Font::GetNamedFont(FontNames::HudFont);
		
		// ** Draws the global HP bar **
		if (m_globalHealthBarVisible)
		{
			const int GLOBAL_BAR_MARGIN_L = 39;
			const int GLOBAL_BAR_MARGIN_R = 37;
			
			int fullHpBarWidth = m_textures.m_hpBarGlobalFull.GetWidth() - GLOBAL_BAR_MARGIN_L - GLOBAL_BAR_MARGIN_R;
			
			Rectangle fullBarSampleRect(0, 0, GLOBAL_BAR_MARGIN_L + fullHpBarWidth * m_globalHealthBarPercentage,
			                            m_textures.m_hpBarFull.GetHeight());
			Rectangle emptyBarSampleRect(fullBarSampleRect.w, 0, m_textures.m_hpBarEmpty.GetWidth() -
			                             fullBarSampleRect.w, m_textures.m_hpBarFull.GetHeight());
			
			Rectangle fullBarTargetRect(m_globalHpBarRectangle.x, m_globalHpBarRectangle.y,
			                            m_globalHpBarRectangle.w * m_globalHealthBarPercentage, m_globalHpBarRectangle.h);
			Rectangle emptyBarTargetRect(fullBarTargetRect.FarX(), m_globalHpBarRectangle.y,
			                             m_globalHpBarRectangle.w - fullBarTargetRect.w, m_globalHpBarRectangle.h);
			
			const glm::vec4 globalHealthBarColor(1.0f, 1.0f, 1.0f, m_globalHealthBarOpacity);
			
			if (emptyBarSampleRect.w > 1E-6f)
			{
				UIRenderer::GetInstance().DrawSprite(m_textures.m_hpBarGlobalEmpty, emptyBarTargetRect,
				                                     emptyBarSampleRect, globalHealthBarColor);
			}
			
			if (fullBarSampleRect.w > 1E-6f)
			{
				UIRenderer::GetInstance().DrawSprite(m_textures.m_hpBarGlobalFull, fullBarTargetRect,
				                                     fullBarSampleRect, globalHealthBarColor);
			}
		}
		
		// ** Draws the HP bar **
		const int BAR_MARGIN_L = 29;
		const int BAR_MARGIN_R = 37;
		int fullHpBarWidth = m_textures.m_hpBarFull.GetWidth() - BAR_MARGIN_L - BAR_MARGIN_R;
		
		float hpPercent = m_hp / m_playerEntity->GetMaxHp();
		
		Rectangle fullHpBarSampleRect(0, 0, BAR_MARGIN_L + fullHpBarWidth * hpPercent,
		                              m_textures.m_hpBarFull.GetHeight());
		Rectangle emptyHpBarSampleRect(fullHpBarSampleRect.w, 0, m_textures.m_hpBarEmpty.GetWidth() -
		                               fullHpBarSampleRect.w, m_textures.m_hpBarFull.GetHeight());
		
		Rectangle fullHpBarTargetRect(m_hpBarRectangle.x, m_hpBarRectangle.y,
		                              m_hpBarRectangle.w * hpPercent, m_hpBarRectangle.h);
		Rectangle emptyHpBarTargetRect(fullHpBarTargetRect.FarX(), m_hpBarRectangle.y,
		                               m_hpBarRectangle.w - fullHpBarTargetRect.w, m_hpBarRectangle.h);
		
		if (emptyHpBarSampleRect.w > 1E-6f)
		{
			UIRenderer::GetInstance().DrawSprite(m_textures.m_hpBarEmpty, emptyHpBarTargetRect,
			                                     emptyHpBarSampleRect, glm::vec4(1.0f));
		}
		
		if (fullHpBarSampleRect.w > 1E-6f)
		{
			UIRenderer::GetInstance().DrawSprite(m_textures.m_hpBarFull, fullHpBarTargetRect,
			                                     fullHpBarSampleRect, glm::vec4(1.0f));
		}
		
		std::string hpString = std::to_string(static_cast<int>(std::round(m_hp)));
		UIRenderer::GetInstance().DrawString(hudFont, hpString, m_hpBarRectangle,
		                                     Alignment::Left, Alignment::Center, glm::vec4(1.0f));
		
		// ** Draws the energy bar **
		float energyPercent = m_energy / PlayerEntity::MAX_ENERGY;
		int fullEnergyBarWidth = m_textures.m_energyBarFull.GetWidth() - BAR_MARGIN_L - BAR_MARGIN_R;
		
		Rectangle fullEnergyBarSampleRect(0, 0, BAR_MARGIN_L + fullEnergyBarWidth *
		                                  energyPercent, m_textures.m_energyBarFull.GetHeight());
		Rectangle emptyEnergyBarSampleRect(fullEnergyBarSampleRect.w, 0, m_textures.m_energyBarEmpty.GetWidth() -
		                                   fullEnergyBarSampleRect.w,
		                                   m_textures.m_energyBarEmpty.GetHeight());
		
		Rectangle fullEnergyBarTargetRect(m_energyBarRectangle.x, m_energyBarRectangle.y,
		                                  m_energyBarRectangle.w * energyPercent, m_energyBarRectangle.h);
		Rectangle emptyEnergyBarTargetRect(fullEnergyBarTargetRect.FarX(), m_energyBarRectangle.y,
		                                   m_energyBarRectangle.w - fullEnergyBarTargetRect.w, m_energyBarRectangle.h);
		
		if (emptyEnergyBarSampleRect.w > 1E-6f)
		{
			UIRenderer::GetInstance().DrawSprite(m_textures.m_energyBarEmpty, emptyEnergyBarTargetRect,
			                                     emptyEnergyBarSampleRect, glm::vec4(1.0f));
		}
		
		if (fullEnergyBarSampleRect.w > 1E-6f)
		{
			UIRenderer::GetInstance().DrawSprite(m_textures.m_energyBarFull, fullEnergyBarTargetRect,
			                                     fullEnergyBarSampleRect, glm::vec4(1.0f));
		}
		
		std::string enegyString = std::to_string(static_cast<int>(std::round(m_energy)));
		UIRenderer::GetInstance().DrawString(hudFont, enegyString, m_energyBarRectangle,
		                                     Alignment::Left, Alignment::Center, glm::vec4(1.0f));
		
		// ** Draws weapon icons **
		for (const WeaponIcon& weaponIcon : m_weaponIcons)
			weaponIcon.Draw(UIRenderer::GetInstance());
		
		std::string weaponInfoString = m_weaponIcons[GetWeaponIndex(m_playerEntity->GetWeaponState())].GetInfoString();
		UIRenderer::GetInstance().DrawString(hudFont, weaponInfoString, m_weaponInfoRectangle,
		                                     Alignment::Center, Alignment::Center, glm::vec4(1.0f));
		
		UIRenderer::GetInstance().DrawRectangle(m_weaponIconsLineRectangle, glm::vec4(1.0f));
		
		if (m_noAmmoOpacity > 0)
		{
			glm::vec4 color(1, 1, 1, glm::min(m_noAmmoOpacity, 1.0f));
			UIRenderer::GetInstance().DrawSprite(m_textures.m_noAmmo, m_noAmmoRectangle, color);
		}
	}
	
	HUDManager::Textures::Textures(const fs::path& dirPath)
	    : m_hpBarFull(Texture2D::FromFile(dirPath / "hp-full.png", 4)),
	      m_hpBarEmpty(Texture2D::FromFile(dirPath / "hp-empty.png", 4)),
	      m_energyBarFull(Texture2D::FromFile(dirPath / "energy-full.png", 4)),
	      m_energyBarEmpty(Texture2D::FromFile(dirPath / "energy-empty.png", 4)),
	      m_hpBarGlobalFull(Texture2D::FromFile(dirPath / "hp-global-full.png", 4)),
	      m_hpBarGlobalEmpty(Texture2D::FromFile(dirPath / "hp-global-empty.png", 4)),
	      m_noAmmo(Texture2D::FromFile(dirPath / "no-ammo.png", 4))
	{
		
	}
}
