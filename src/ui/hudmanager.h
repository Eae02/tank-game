#pragma once

#include "../graphics/ui/uirenderer.h"
#include "../graphics/gl/framebuffer.h"
#include "../graphics/gl/texture2d.h"
#include "../graphics/gl/buffer.h"
#include "../graphics/gl/vertexarray.h"
#include "../utils/memory/stackobject.h"
#include "../utils/filesystem.h"
#include "pausemenu.h"

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
		};
		
		float m_hp;
		
		float m_blurAmount = 0;
		
		class IMainRenderer& m_mainRenderer;
		
		GLsizei m_vertexCount;
		Buffer m_vertexBuffer;
		VertexArray m_vertexArray;
		
		Textures m_textures;
		
		PauseMenu m_pauseMenu;
		
		Rectangle m_hpBarRectangle;
		
		StackObject<Framebuffer> m_framebuffer;
		StackObject<Texture2D> m_fbTexture;
		
		ShaderProgram m_shader;
	};
}
