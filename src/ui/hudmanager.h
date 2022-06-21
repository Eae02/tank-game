#pragma once

#include "../graphics/ui/uirenderer.h"
#include "../graphics/gl/framebuffer.h"
#include "../graphics/gl/texture2d.h"
#include "../graphics/gl/buffer.h"
#include "../graphics/gl/vertexinputstate.h"
#include "../utils/filesystem.h"
#include "../world/entities/playerweaponstate.h"
#include "weaponicon.h"
#include "pausemenu.h"
#include "levelcompletemenu.h"

#include <memory>

namespace TankGame
{
	class HUDManager
	{
	public:
		explicit HUDManager(class IMainRenderer& mainRenderer);
		
		void OnResize(GLsizei width, GLsizei height);
		
		void Update(const class UpdateInfo& updateInfo);
		
		void DrawHUD();
		
		void SetPlayerEntity(const class PlayerEntity* playerEntity);
		
		inline void ShowLevelComplete(std::string nextLevelName)
		{
			m_levelCompleteMenu.Show(std::move(nextLevelName));
		}
		
		inline void ShowPauseMenu()
		{
			m_pauseMenu.Show();
		}
		
		inline bool IsPaused() const
		{
			return m_pauseMenu.IsShown() || m_levelCompleteMenu.IsShown();
		}
		
		inline void SetShouldQuitToEditor(bool shouldQuitToEditor)
		{ m_pauseMenu.SetShouldQuitToEditor(shouldQuitToEditor); }
		
		void SetQuitCallback(std::function<void()> callback)
		{
			m_levelCompleteMenu.SetQuitCallback(callback);
			m_pauseMenu.SetQuitCallback(std::move(callback));
		}
		
		inline void ShowNoAmmoText()
		{ m_noAmmoOpacity = 1.5f; }
		
		inline void SetShowGlobalHealthBar(bool showGlobalHealthBar)
		{ m_globalHealthBarVisible = showGlobalHealthBar; }
		inline void SetGlobalHealthBarPercentage(float percentage)
		{ m_globalHealthBarTargetPercentage = glm::clamp(percentage, 0.0f, 1.0f); }
		
		static int GetWeaponIndex(const class PlayerWeaponState& weaponState);
		
	private:
		void LayoutElements(GLsizei screenWidth, GLsizei screenHeight);
		void DrawHUDElements();
		
		const class PlayerEntity* m_playerEntity = nullptr;
		
		class Textures
		{
		public:
			Textures(const fs::path& dirPath);
			
			Texture2D m_hpBarFull;
			Texture2D m_hpBarEmpty;
			Texture2D m_energyBarFull;
			Texture2D m_energyBarEmpty;
			Texture2D m_hpBarGlobalFull;
			Texture2D m_hpBarGlobalEmpty;
			Texture2D m_noAmmo;
		};
		
		float m_hp;
		float m_energy;
		
		float m_blurAmount = 0;
		
		float m_noAmmoOpacity = 0;
		
		bool m_globalHealthBarVisible = false;
		float m_globalHealthBarTargetPercentage = 1.0f;
		float m_globalHealthBarPercentage = 1.0f;
		float m_globalHealthBarOpacity = 0.0f;
		
		class IMainRenderer& m_mainRenderer;
		
		std::array<WeaponIcon, 2> m_weaponIcons;
		Rectangle m_weaponIconsLineRectangle;
		Rectangle m_weaponInfoRectangle;
		
		GLsizei m_vertexCount;
		Buffer m_vertexBuffer;
		VertexInputState m_vertexInputState;
		
		Textures m_textures;
		
		PauseMenu m_pauseMenu;
		LevelCompleteMenu m_levelCompleteMenu;
		
		Rectangle m_contentsRect;
		
		Rectangle m_noAmmoRectangle;
		Rectangle m_hpBarRectangle;
		Rectangle m_energyBarRectangle;
		Rectangle m_globalHpBarRectangle;
		
		std::unique_ptr<Framebuffer> m_framebuffer;
		std::unique_ptr<Texture2D> m_fbTexture;
		
		ShaderProgram m_shader;
	};
}
