#pragma once

#include "../gl/texture2d.h"
#include "../gl/shaderprogram.h"
#include "../gl/vertexarray.h"
#include "../../rectangle.h"
#include "../../utils/memory/stackobject.h"

#include <glm/glm.hpp>

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
		
		void DrawString(const class Font& font, const std::string& string, Rectangle rectangle, Alignment alignX,
		                Alignment alignY, const glm::vec4& color, float scale = 1) const;
		
		void DrawString(const class Font& font, const std::u32string& string, Rectangle rectangle, Alignment alignX,
		                Alignment alignY, const glm::vec4& color, float scale = 1) const;
		
		void DrawRectangle(const Rectangle& rectangle, const glm::vec4& color) const;
		void DrawQuad(const glm::mat3& transform, const glm::vec4& color) const;
		
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
		
	private:
		static void LoadSpriteShader();
		static void LoadQuadShader();
		static void LoadLineShader();
		static void LoadTextShader();
		
		static std::unique_ptr<UIRenderer> s_instance;
		
		struct SpriteShader
		{
			StackObject<ShaderProgram> m_shader;
			int m_sampleRectMinLocation;
			int m_sampleRectMaxLocation;
			int m_targetRectMinLocation;
			int m_targetRectMaxLocation;
			int m_shadeLocation;
		};
		static SpriteShader s_spriteShader;
		
		struct QuadShader
		{
			StackObject<ShaderProgram> m_shader;
			int m_colorLocation;
			int m_transformLocation;
		};
		static QuadShader s_quadShader;
		
		struct LineShader
		{
			StackObject<ShaderProgram> m_shader;
			int m_vertex1Location;
			int m_vertex2Location;
			int m_colorLocation;
		};
		static LineShader s_lineShader;
		
		struct TextShader
		{
			StackObject<ShaderProgram> m_shader;
			int m_offsetLocation;
			int m_sizeLocation;
			int m_colorLocation;
		};
		static TextShader s_textShader;
		
		VertexArray m_lineVAO;
		
		int m_windowWidth = 1;
		int m_windowHeight = 1;
	};
}
