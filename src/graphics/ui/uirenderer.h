#pragma once

#include "../gl/texture2d.h"
#include "../gl/shaderprogram.h"
#include "../../rectangle.h"


namespace TankGame
{
	enum class Alignment
	{
		Left = 0,
		Center = 1,
		Right = 2,
		Top = 2,
		Bottom = 0
	};
	
	class UIRenderer
	{
	public:
		inline void DrawSprite(const Texture2D& texture, const Rectangle& targetRectangle, const glm::vec4& shade) const
		{
			DrawSprite(texture, targetRectangle, Rectangle(0.0f, 0.0f, texture.GetWidth(), texture.GetHeight()), shade);
		}
		
		void DrawSprite(const Texture2D& texture, const Rectangle& targetRectangle, const Rectangle& sampleRectangle,
		                const glm::vec4& shade) const;
		
		glm::vec2 DrawString(const class Font& font, const std::string& string, Rectangle rectangle,
		                     Alignment alignX, Alignment alignY, const glm::vec4& color, float scale = 1) const;
		
		void DrawRectangle(const Rectangle& rectangle, const glm::vec4& color) const;
		void DrawQuad(const glm::mat3& transform, const glm::vec4& color) const;
		
		void DrawQuad(const std::array<glm::vec2, 4>& corners, const glm::vec4& color) const;
		
		void DrawQuadBorder(const glm::mat3& transform, const glm::vec4& color) const;
		
		void DrawLine(const glm::vec2& a, const glm::vec2& b, const glm::vec4& color) const;
		
		void SetWindowDimensions(int width, int height);
		
		inline int GetWindowWidth() const
		{ return m_windowWidth; }
		inline int GetWindowHeight() const
		{ return m_windowHeight; }
		
		inline static UIRenderer& GetInstance()
		{ return *s_instance; }
		inline static void SetInstance(std::unique_ptr<UIRenderer>&& instance)
		{ s_instance = std::move(instance); }
		static void SetSingletonWindowDimensions(int newWidth, int newHeight);
		
	private:
		static void LoadSpriteShader();
		static void LoadQuadShader();
		static void LoadLineShader();
		
		static std::unique_ptr<UIRenderer> s_instance;
		
		struct SpriteShader
		{
			std::unique_ptr<ShaderProgram> m_shader;
			int m_sampleRectMinLocation;
			int m_sampleRectMaxLocation;
			int m_targetRectMinLocation;
			int m_targetRectMaxLocation;
			int m_shadeLocation;
			int m_redToAlphaLocation;
		};
		static SpriteShader s_spriteShader;
		
		struct QuadShader
		{
			std::unique_ptr<ShaderProgram> m_shader;
			int m_colorLocation;
			int m_cornersLocation;
		};
		static QuadShader s_quadShader;
		
		struct LineShader
		{
			std::unique_ptr<ShaderProgram> m_shader;
			int m_vertex1Location;
			int m_vertex2Location;
			int m_colorLocation;
		};
		static LineShader s_lineShader;
		
		int m_windowWidth = 1;
		int m_windowHeight = 1;
	};
}
