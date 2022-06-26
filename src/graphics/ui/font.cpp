#include "font.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"

#include <cstring>
#include <stb_rect_pack.h>
#include <stb_truetype.h>

namespace TankGame
{
	static constexpr uint32_t MIN_CHAR = 32;
	static constexpr uint32_t MAX_CHAR = 126;
	
	constexpr int Font::NUM_NAMED_FONTS;
	
	Font::Font(Texture2D texture, float size)
	    : m_glyphs(std::make_unique<Glyph[]>(MAX_CHAR - MIN_CHAR + 1)),
		  m_texture(std::move(texture)), m_size(size) { }
	
	Font Font::Render(const fs::path& path, float size)
	{
		std::string ttfContents = ReadFileContents(path);
		uint8_t* ttfContentsPtr = reinterpret_cast<uint8_t*>(ttfContents.data());
		
		stbtt_fontinfo fontInfo;
		if (!stbtt_InitFont(&fontInfo, ttfContentsPtr, stbtt_GetFontOffsetForIndex(ttfContentsPtr, 0)))
			Panic("Failed to render font: '" + path.string() + "'.");
		
		float scale = stbtt_ScaleForPixelHeight(&fontInfo, size);
		
		struct RenderedCharacter
		{
			uint32_t c;
			int width;
			int height;
			int xOffset;
			int yOffset;
			std::unique_ptr<uint8_t, FreeDeleter> bitmap;
		};
		
		size_t numRenderedCharacters = 0;
		std::array<RenderedCharacter, MAX_CHAR - MIN_CHAR + 1> renderedCharacters;
		std::array<stbrp_rect, MAX_CHAR - MIN_CHAR + 1> rectangles;
		
		for (uint32_t c = MIN_CHAR; c <= MAX_CHAR; c++)
		{
			int charWidth, charHeight, xOffset, yOffset;
			uint8_t* bitmap = stbtt_GetCodepointBitmap(&fontInfo, 0, scale, c, &charWidth, &charHeight, &xOffset, &yOffset);
			if (bitmap != nullptr)
			{
				rectangles.at(numRenderedCharacters).w = charWidth + 2;
				rectangles.at(numRenderedCharacters).h = charHeight + 2;
				renderedCharacters.at(numRenderedCharacters) = RenderedCharacter
				{
					.c = c,
					.width = charWidth,
					.height = charHeight,
					.xOffset = xOffset,
					.yOffset = yOffset,
					.bitmap = std::unique_ptr<uint8_t, FreeDeleter>(bitmap)
				};
				numRenderedCharacters++;
			}
		}
		
		std::vector<stbrp_node> nodes;
		constexpr int SIZE_INCREMENT = 64;
		int width = 128;
		int height = 128;
		while (true)
		{
			if (nodes.size() < (size_t)width)
				nodes.resize(width);
			stbrp_context packContext;
			stbrp_init_target(&packContext, width, height, nodes.data(), nodes.size());
			if (stbrp_pack_rects(&packContext, rectangles.data(), numRenderedCharacters))
				break;
			if (height < width)
				height += SIZE_INCREMENT;
			else
				width += SIZE_INCREMENT;
		}
		
		const size_t textureDataBytes = (size_t)width * (size_t)height;
		std::unique_ptr<char, FreeDeleter> textureData(static_cast<char*>(std::calloc(textureDataBytes, 1)));
		for (size_t i = 0; i < numRenderedCharacters; i++)
		{
			const size_t maxSrcOffset = renderedCharacters[i].width * renderedCharacters[i].height;
			for (int row = 0; row < renderedCharacters[i].height; row++)
			{
				const int dstY = height - 1 - (rectangles[i].y + (renderedCharacters[i].height - 1 - row) + 1);
				const size_t srcOffset = row * renderedCharacters[i].width;
				const size_t dstOffset = (size_t)dstY * (size_t)width + rectangles[i].x + 1;
				
				assert(srcOffset + renderedCharacters[i].width <= maxSrcOffset);
				assert(dstOffset + renderedCharacters[i].width <= textureDataBytes);
				
				std::memcpy(textureData.get() + dstOffset, renderedCharacters[i].bitmap.get() + srcOffset, renderedCharacters[i].width);
			}
		}
		Font font(Texture2D(width, height, 1, TextureFormat::R8), size);
		font.m_texture.SetData({ textureData.get(), textureDataBytes });
		font.m_texture.SetupMipmapping(false);
		
#ifndef __EMSCRIPTEN__
		glTextureParameteri(font.m_texture.GetID(), GL_TEXTURE_SWIZZLE_R, GL_ONE);
		glTextureParameteri(font.m_texture.GetID(), GL_TEXTURE_SWIZZLE_G, GL_ONE);
		glTextureParameteri(font.m_texture.GetID(), GL_TEXTURE_SWIZZLE_B, GL_ONE);
		glTextureParameteri(font.m_texture.GetID(), GL_TEXTURE_SWIZZLE_A, GL_RED);
#endif
		
		int ascent, descent;
		stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, nullptr);
		font.m_ascent = (float)ascent * scale;
		font.m_descent = (float)descent * scale;
		
		glm::vec2 uvScale(1.0f / (float)width, 1.0f / (float)height);
		
		for (uint32_t c = MIN_CHAR; c <= MAX_CHAR; c++)
		{
			int advanceWidth, leftBearing;
			stbtt_GetCodepointHMetrics(&fontInfo, c, &advanceWidth, &leftBearing);
			font.m_glyphs[c - MIN_CHAR] = Glyph { .advance = (float)advanceWidth * scale };
		}
		
		for (size_t i = 0; i < numRenderedCharacters; i++)
		{
			Glyph& glyph = font.m_glyphs[renderedCharacters[i].c - MIN_CHAR];
			glyph.hasImage = true;
			glyph.bearing.x = renderedCharacters[i].xOffset;
			glyph.bearing.y = -renderedCharacters[i].height - renderedCharacters[i].yOffset;
			glyph.size = glm::vec2(renderedCharacters[i].width, renderedCharacters[i].height);
			glyph.uvMin = glm::vec2(rectangles[i].x + 1, rectangles[i].y + 1) * uvScale;
			glyph.uvMax = glm::vec2(rectangles[i].x + rectangles[i].w - 1, rectangles[i].y + rectangles[i].h - 1) * uvScale;
		}
		
		return font;
	}
	
	const Font::Glyph* Font::TryGetGlyph(uint32_t charCode) const
	{
		if (charCode >= MIN_CHAR && charCode <= MAX_CHAR)
			return &m_glyphs[charCode - MIN_CHAR];
		return nullptr;
	}
	
	static std::unique_ptr<Font> namedFonts[Font::NUM_NAMED_FONTS];
	
	static std::pair<float, const char*> namedFontFilesAndSizes[Font::NUM_NAMED_FONTS] =
	{
		{ 20, "ui.ttf" },
		{ 24, "ui.ttf" },
		{ 20, "orbitron.ttf" },
		{ 40, "orbitron.ttf" },
		{ 12, "mono.ttf" },
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
			const fs::path fontPath = resDirectoryPath / "fonts" / namedFontFilesAndSizes[index].second;
			namedFonts[index] = std::make_unique<Font>(Font::Render(fontPath, namedFontFilesAndSizes[index].first));
		}
		
		return *namedFonts[index];
	}
	
	glm::vec2 Font::MeasureString(const std::string& string) const
	{
		float x = 0;
		for (uint32_t c : string)
		{
			if (const Glyph* glyph = TryGetGlyph(c))
				x += glyph->advance;
		}
		return { x, m_ascent };
	}
}
