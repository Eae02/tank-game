#pragma once

#include <string>
#include <memory>
#include <optional>
#include <glm/glm.hpp>

#include "../gl/texture2d.h"

namespace TankGame
{
	enum class FontNames
	{
		StandardUI = 0,
		MenuTitle = 1,
		MenuButtonFont = 2,
		HudFont = 2,
		BigHudFont = 3,
		ButtonFont = 3,
		Dev = 4
	};
	
	class Font
	{
	public:
		struct Glyph
		{
			float advance;
			glm::vec2 bearing;
			glm::vec2 size;
			glm::vec2 uvMin;
			glm::vec2 uvMax;
			bool hasImage;
		};
		
		static Font Render(const fs::path& path, float size);
		
		const Glyph* TryGetGlyph(uint32_t charCode) const;
		
		glm::vec2 MeasureString(const std::string& string) const;
		
		static const Font& GetNamedFont(FontNames name);
		static void DestroyFonts();
		
		const Texture2D& GetTexture() const
		{ return m_texture; }
		
		inline float GetSize() const
		{ return m_size; }
		
		static constexpr int NUM_NAMED_FONTS = 5;
		
	private:
		Font(Texture2D texture, float size);
		
		std::unique_ptr<Glyph[]> m_glyphs;
		
		Texture2D m_texture;
		
		float m_size;
		float m_ascent;
		float m_descent;
	};
}
