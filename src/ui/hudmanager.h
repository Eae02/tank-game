#pragma once

#include "../graphics/ui/uirenderer.h"
#include "../graphics/gl/framebuffer.h"
#include "../graphics/gl/texture2d.h"
#include "../graphics/gl/buffer.h"
#include "../graphics/gl/vertexarray.h"
#include "../utils/filesystem.h"
#include "../world/entities/playerweaponstate.h"
#include "weaponicon.h"
#include "pausemenu.h"

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
		
		inline void ShowPauseMenu()
		{ m_pauseMenu.Show(); }
		
		inline bool IsPaused() const
		{ return m_pauseMenu.IsShown(); }
		
		inline void SetShouldQuitToEditor(bool shouldQuitToEditor)
		{ m_pauseMenu.SetShouldQuitToEditor(shouldQuitToEditor); }
		
		inline void SetQuitCallback(std::function<void()> callback)
		{ m_pauseMenu.SetQuitCallback(std::move(callback)); }
		
		inline void ShowNoAmmoText()
		{ m_noAmmoOpacity = 1.5f; }
		
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
			Texture2D m_noAmmo;
		};
		
		float m_hp;
		float m_energy;
		
		float m_blurAmount = 0;
		
		float m_noAmmoOpacity = 0;
		
		class IMainRenderer& m_mainRenderer;
		
		std::array<WeaponIcon, 2> m_weaponIcons;
		Rectangle m_weaponIconsLineRectangle;
		Rectangle m_weaponInfoRectangle;
		
		GLsizei m_vertexCount;
		Buffer m_vertexBuffer;
		VertexArray m_vertexArray;
		
		Textures m_textures;
		
		PauseMenu m_pauseMenu;
		
		Rectangle m_contentsRect;
		
		Rectangle m_noAmmoRectangle;
		Rectangle m_hpBarRectangle;
		Rectangle m_energyBarRectangle;
		
		std::unique_ptr<Framebuffer> m_framebuffer;
		std::unique_ptr<Texture2D> m_fbTexture;
		
		ShaderProgram m_shader;
	};
}
