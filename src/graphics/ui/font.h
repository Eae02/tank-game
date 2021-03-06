#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_MODULE_H

#include <string>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>

#include "../gl/texture2d.h"

namespace TankGame
{
	extern FT_Library theFTLibrary;
	
	enum class FontNames
	{
		StandardUI = 0,
		MenuTitle = 1,
		MenuButtonFont = 2,
		HudFont = 2,
		BigHudFont = 3,
		ButtonFont = 3
	};
	
	class Font
	{
		class FontFaceDeleter
		{
		public:
			using pointer = FT_Face;
			
			inline void operator()(FT_Face face)
			{ FT_Done_Face(face); }
		};
		
	public:
		struct Glyph
		{
			float m_advance;
			glm::ivec2 m_bearing;
			
			std::unique_ptr<Texture2D> m_texture;
		};
		
		Font(const fs::path& path, FT_UInt size);
		
		const Glyph* TryGetGlyph(uint32_t charCode) const;
		
		glm::vec2 MeasureString(const std::u32string& string) const;
		
		static const Font& GetNamedFont(FontNames name);
		static void DestroyFonts();
		
		inline FT_UInt GetSize() const
		{ return m_size; }
		
		static constexpr int NUM_NAMED_FONTS = 4;
		
	private:
		static Glyph RenderGlyph(FT_GlyphSlot ftGlyph);
		
		mutable std::unordered_map<uint32_t, Glyph> m_glyphs;
		
		std::unique_ptr<FT_Face, FontFaceDeleter> m_fontFace;
		
		FT_UInt m_size;
	};
}
