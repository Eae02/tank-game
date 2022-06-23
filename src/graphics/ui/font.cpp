#include "font.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"

namespace TankGame
{
	constexpr int Font::NUM_NAMED_FONTS;
	
	FT_Library theFTLibrary;
	
	FT_Face OpenFontFace(const std::string& path, FT_UInt size)
	{
		FT_Face fontFace;
		
		FT_Error state = FT_New_Face(theFTLibrary, path.c_str(), 0, &fontFace);
		if (state == FT_Err_Unknown_File_Format)
			Panic("Unsupported font format: '" + path + "'.");
		else if (state != 0)
			Panic("Error reading font file: '" + path + "'.");
		
		FT_Set_Pixel_Sizes(fontFace, 0, size);
		
		return fontFace;
	}
	
	Font::Font(const fs::path& path, FT_UInt size)
	    : m_fontFace(OpenFontFace(path.string(), size), FontFaceDeleter()), m_size(size)
	{
		for (uint32_t i = 0x20; i <= 0x7F; i++)
			TryGetGlyph(i);
	}
	
	const Font::Glyph* Font::TryGetGlyph(uint32_t charCode) const
	{
		auto glyphPos = m_glyphs.find(charCode);
		if (glyphPos != m_glyphs.end())
			return &glyphPos->second;
		
		if (FT_Load_Char(m_fontFace.get(), charCode, FT_LOAD_RENDER) != 0)
			return nullptr;
		
		auto insertResult = m_glyphs.emplace(charCode, RenderGlyph(m_fontFace.get()->glyph));
		return &insertResult.first->second;
	}
	
	Font::Glyph Font::RenderGlyph(FT_GlyphSlot ftGlyph)
	{
		Font::Glyph glyph;
		
		glyph.m_advance = ftGlyph->advance.x / 64.0f;
		glyph.m_bearing = glm::ivec2(ftGlyph->bitmap_left, ftGlyph->bitmap_top);
		
		if (ftGlyph->bitmap.width != 0 && ftGlyph->bitmap.rows != 0)
		{
			glyph.m_texture = std::make_unique<Texture2D>(ftGlyph->bitmap.width, ftGlyph->bitmap.rows, 1, TextureFormat::R8);
			
			glyph.m_texture->SetData({
				reinterpret_cast<char*>(ftGlyph->bitmap.buffer),
				ftGlyph->bitmap.width * ftGlyph->bitmap.rows
			});
			
			glyph.m_texture->SetWrapMode(GL_CLAMP_TO_EDGE);
		}
		
		return glyph;
	}
	
	static std::unique_ptr<Font> namedFonts[Font::NUM_NAMED_FONTS];
	
	static std::string fontFileNames[Font::NUM_NAMED_FONTS] =
	{
		"ui.ttf",
		"ui.ttf",
		"orbitron.ttf",
		"orbitron.ttf",
		"mono.ttf"
	};
	
	static FT_UInt namedFontSizes[Font::NUM_NAMED_FONTS] =
	{
		16,
		24,
		20,
		40,
		12
	};
	
	void Font::DestroyFonts()
	{
		for (std::unique_ptr<Font>& font : namedFonts)
			font = nullptr;
	}
	
	const Font& Font::GetNamedFont(FontNames name)
	{
		int index = static_cast<int>(name);
		
		if (namedFonts[index] == nullptr)
		{
			const fs::path fontPath = resDirectoryPath / "fonts" / fontFileNames[index];
			namedFonts[index] = std::make_unique<Font>(fontPath, namedFontSizes[index]);
		}
		
		return *namedFonts[index];
	}
	
	glm::vec2 Font::MeasureString(const std::string& string) const
	{
		float x = 0;
		float maxHeight = 0;
		
		for (uint32_t c : string)
		{
			if (const Glyph* glyph = TryGetGlyph(c))
			{
				x += glyph->m_advance;
				if (glyph->m_texture != nullptr)
					maxHeight = glm::max<float>(maxHeight, glyph->m_texture->GetHeight());
			}
		}
		
		return { x, maxHeight };
	}
}
