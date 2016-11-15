#include "font.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"

namespace TankGame
{
	FT_Library theFTLibrary;
	
	FT_Face OpenFontFace(const std::string& path, FT_UInt size)
	{
		FT_Face fontFace;
		
		FT_Error state = FT_New_Face(theFTLibrary, path.c_str(), 0, &fontFace);
		if (state == FT_Err_Unknown_File_Format)
			throw std::runtime_error("Unsupported font format.");
		else if (state != 0)
			throw std::runtime_error("Error reading font file.");
		
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
			glyph.m_texture.Construct(ftGlyph->bitmap.width, ftGlyph->bitmap.rows, 1, GL_R8);
			glTextureSubImage2D(glyph.m_texture->GetID(), 0, 0, 0, ftGlyph->bitmap.width, ftGlyph->bitmap.rows,
			                    GL_RED, GL_UNSIGNED_BYTE, ftGlyph->bitmap.buffer);
			
			glyph.m_texture->SetWrapMode(GL_CLAMP_TO_EDGE);
		}
		
		return glyph;
	}
	
	static StackObject<Font> namedFonts[3];
	
	static std::string fontFileNames[3] =
	{
		"ui.ttf",
		"ui.ttf",
		"orbitron.ttf"
	};
	
	static FT_UInt namedFontSizes[3] =
	{
		16,
		24,
		20
	};
	
	void Font::DestroyFonts()
	{
		for (StackObject<Font>& font : namedFonts)
			font.Destroy();
	}
	
	const Font& Font::GetNamedFont(FontNames name)
	{
		int index = static_cast<int>(name);
		
		if (namedFonts[index].IsNull())
		{
			namedFonts[index].Construct(GetResDirectory() / "fonts" / fontFileNames[index], namedFontSizes[index]);
		}
		
		return *namedFonts[index];
	}
	
	glm::vec2 Font::MeasureString(const std::u32string& string) const
	{
		float x = 0;
		float maxHeight = 0;
		
		for (uint32_t c : string)
		{
			if (const Glyph* glyph = TryGetGlyph(c))
			{
				x += glyph->m_advance;
				if (!glyph->m_texture.IsNull())
					maxHeight = glm::max<float>(maxHeight, glyph->m_texture->GetHeight());
			}
		}
		
		return { x, maxHeight };
	}
}
