#include "uirenderer.h"
#include "font.h"
#include "../gl/shadermodule.h"
#include "../gl/vertexinputstate.h"
#include "../quadmesh.h"
#include "../../utils/utils.h"
#include "../../utils/mathutils.h"
#include "../../utils/ioutils.h"

#include <imgui.h>

namespace TankGame
{
	std::unique_ptr<UIRenderer> UIRenderer::s_instance;
	
	UIRenderer::SpriteShader UIRenderer::s_spriteShader;
	
	UIRenderer::QuadShader UIRenderer::s_quadShader;
	UIRenderer::LineShader UIRenderer::s_lineShader;
	
	void UIRenderer::LoadSpriteShader()
	{
		s_spriteShader.m_shader = std::make_unique<ShaderProgram>(
			ShaderModule::FromResFile("ui/uisprite.vs.glsl"), ShaderModule::FromResFile("ui/uisprite.fs.glsl"));
		CallOnClose([] { s_spriteShader.m_shader = nullptr; });
		
		s_spriteShader.m_shader->SetTextureBinding("spriteTex", 0);
		s_spriteShader.m_sampleRectMinLocation = s_spriteShader.m_shader->GetUniformLocation("sampleRectMin");
		s_spriteShader.m_sampleRectMaxLocation = s_spriteShader.m_shader->GetUniformLocation("sampleRectMax");
		s_spriteShader.m_targetRectMinLocation = s_spriteShader.m_shader->GetUniformLocation("targetRectMin");
		s_spriteShader.m_targetRectMaxLocation = s_spriteShader.m_shader->GetUniformLocation("targetRectMax");
		s_spriteShader.m_shadeLocation = s_spriteShader.m_shader->GetUniformLocation("shade");
		s_spriteShader.m_redToAlphaLocation = glGetUniformLocation(s_spriteShader.m_shader->GetID(), "redToAlpha");
	}
	
	void UIRenderer::LoadQuadShader()
	{
		s_quadShader.m_shader = std::make_unique<ShaderProgram>(
			ShaderModule::FromResFile("ui/uiquad.vs.glsl"), ShaderModule::FromResFile("ui/uiquad.fs.glsl"));
		CallOnClose([] { s_quadShader.m_shader = nullptr; });
		
		s_quadShader.m_colorLocation = s_quadShader.m_shader->GetUniformLocation("color");
		s_quadShader.m_cornersLocation = s_quadShader.m_shader->GetUniformLocation("corners");
	}
	
	void UIRenderer::LoadLineShader()
	{
		s_lineShader.m_shader = std::make_unique<ShaderProgram>(
			ShaderModule::FromResFile("ui/uiline.vs.glsl"), ShaderModule::FromResFile("ui/uiquad.fs.glsl"));
		CallOnClose([] { s_lineShader.m_shader = nullptr; });
		
		s_lineShader.m_vertex1Location = s_lineShader.m_shader->GetUniformLocation("vertices[0]");
		s_lineShader.m_vertex2Location = s_lineShader.m_shader->GetUniformLocation("vertices[1]");
		s_lineShader.m_colorLocation = s_lineShader.m_shader->GetUniformLocation("color");
	}
	
	void UIRenderer::DrawSprite(const Texture2D& texture, const Rectangle& targetRectangle,
	                            const Rectangle& sampleRectangle, const glm::vec4& shade) const
	{
		if (s_spriteShader.m_shader == nullptr)
			LoadSpriteShader();
		s_spriteShader.m_shader->Use();
		
		if (s_spriteShader.m_redToAlphaLocation != -1)
			glUniform1i(s_spriteShader.m_redToAlphaLocation, 0);
		
		texture.Bind(0);
		
		glUniform2f(s_spriteShader.m_sampleRectMinLocation,
		            sampleRectangle.x / texture.GetWidth(), sampleRectangle.y / texture.GetHeight());
		glUniform2f(s_spriteShader.m_sampleRectMaxLocation,
		            sampleRectangle.FarX() / texture.GetWidth(), sampleRectangle.FarY() / texture.GetHeight());
		
		glm::vec2 targetRectMin(targetRectangle.x / m_windowWidth, targetRectangle.y / m_windowHeight);
		glm::vec2 targetRectMax(targetRectangle.FarX() / m_windowWidth, targetRectangle.FarY() / m_windowHeight);
		
		targetRectMin = targetRectMin * 2.0f - glm::vec2(1.0f);
		targetRectMax = targetRectMax * 2.0f - glm::vec2(1.0f);
		
		glUniform2f(s_spriteShader.m_targetRectMinLocation, targetRectMin.x, targetRectMin.y);
		glUniform2f(s_spriteShader.m_targetRectMaxLocation, targetRectMax.x, targetRectMax.y);
		
		glUniform4fv(s_spriteShader.m_shadeLocation, 1, reinterpret_cast<const GLfloat*>(&shade));
		
		QuadMesh::GetInstance().BindVAO();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	void UIRenderer::SetWindowDimensions(int width, int height)
	{
		m_windowWidth = width;
		m_windowHeight = height;
	}
	
	void UIRenderer::SetSingletonWindowDimensions(int newWidth, int newHeight)
	{
		if (s_instance != nullptr)
			s_instance->SetWindowDimensions(newWidth, newHeight);
	}
	
	void UIRenderer::DrawRectangle(const Rectangle& rectangle, const glm::vec4& color) const
	{
		std::array<glm::vec2, 4> corners = { rectangle.NearPos(), rectangle.NearXFarY(), rectangle.FarXNearY(), rectangle.FarPos() };
		
		DrawQuad(corners, color);
	}
	
	void UIRenderer::DrawQuad(const glm::mat3& transform, const glm::vec4& color) const
	{
		std::array<glm::vec2, 4> corners = { glm::vec2 { -1, -1 }, glm::vec2 { -1, 1 }, glm::vec2 { 1, -1 }, glm::vec2 { 1, 1 } };
		
		for (glm::vec2& corner : corners)
			corner = glm::vec2(transform * glm::vec3(corner, 1.0f));
		
		DrawQuad(corners, color);
	}
	
	void UIRenderer::DrawQuad(const std::array<glm::vec2, 4>& corners, const glm::vec4& color) const
	{
		if (s_quadShader.m_shader == nullptr)
			LoadQuadShader();
		s_quadShader.m_shader->Use();
		
		float cornersNDC[8];
		for (size_t i = 0; i < 4; i++)
		{
			cornersNDC[i * 2 + 0] = corners[i].x * 2.0f / (float)m_windowWidth - 1.0f;
			cornersNDC[i * 2 + 1] = corners[i].y * 2.0f / (float)m_windowHeight - 1.0f;
		}
		
		glUniform2fv(s_quadShader.m_cornersLocation, 4, reinterpret_cast<const GLfloat*>(&cornersNDC));
		
		glUniform4fv(s_quadShader.m_colorLocation, 1, &color.x);
		
		QuadMesh::GetInstance().BindVAO();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	void UIRenderer::DrawQuadBorder(const glm::mat3& transform, const glm::vec4& color) const
	{
		glm::vec2 quadVertices[4] = { { -1, -1 }, { -1, 1 }, { 1, 1 }, { 1, -1 } };
		
		for (int i = 0; i < 4; i++)
			quadVertices[i] = glm::vec2(transform * glm::vec3(quadVertices[i], 1.0f));
		
		for (int i = 0; i < 4; i++)
			DrawLine(quadVertices[i], quadVertices[(i + 1) % 4], color);
	}
	
	void UIRenderer::DrawLine(const glm::vec2& a, const glm::vec2& b, const glm::vec4& color) const
	{
		if (s_lineShader.m_shader== nullptr)
			LoadLineShader();
		s_lineShader.m_shader->Use();
		
		glm::vec2 vertex1 = (2.0f * a) / glm::vec2(m_windowWidth, m_windowHeight) - glm::vec2(1);
		glm::vec2 vertex2 = (2.0f * b) / glm::vec2(m_windowWidth, m_windowHeight) - glm::vec2(1);
		
		glUniform2f(s_lineShader.m_vertex1Location, vertex1.x, vertex1.y);
		glUniform2f(s_lineShader.m_vertex2Location, vertex2.x, vertex2.y);
		glUniform4fv(s_lineShader.m_colorLocation, 1, reinterpret_cast<const GLfloat*>(&color));
		
		VertexInputState::BindEmpty();
		glDrawArrays(GL_LINES, 0, 2);
	}
	
	glm::vec2 UIRenderer::DrawString(const Font& font, const std::string& string, Rectangle rectangle,
	                                 Alignment alignX, Alignment alignY, const glm::vec4& color, float scale) const
	{
		if (s_spriteShader.m_shader == nullptr)
			LoadSpriteShader();
		s_spriteShader.m_shader->Use();
		
		if (s_spriteShader.m_redToAlphaLocation != -1)
			glUniform1i(s_spriteShader.m_redToAlphaLocation, 1);
		
		glm::vec2 size = font.MeasureString(string) * scale;
		
		glm::vec2 position;
		position.x = rectangle.x + (static_cast<int>(alignX) / 2.0f) * (rectangle.w - size.x);
		position.y = rectangle.y + (static_cast<int>(alignY) / 2.0f) * (rectangle.h - size.y);
		
		glUniform4fv(s_spriteShader.m_shadeLocation, 1, reinterpret_cast<const GLfloat*>(&color));
		
		glm::vec2 halfPixelSize(2.0f / (float)m_windowWidth, 2.0f / (float)m_windowHeight);
		
		QuadMesh::GetInstance().BindVAO();
		
		font.GetTexture().Bind(0);
		
		for (size_t i = 0; i < string.length(); i++)
		{
			const Font::Glyph* glyph = font.TryGetGlyph(string[i]);
			
			if (glyph == nullptr)
				continue;
			
			if (glyph->hasImage)
			{
				glUniform2f(s_spriteShader.m_sampleRectMinLocation, glyph->uvMin.x, glyph->uvMin.y);
				glUniform2f(s_spriteShader.m_sampleRectMaxLocation, glyph->uvMax.x, glyph->uvMax.y);
				
				glm::vec2 targetRectMin = position + glyph->bearing * scale;
				glm::vec2 targetRectMax = targetRectMin + glyph->size * scale;
				
				targetRectMin = targetRectMin * halfPixelSize - glm::vec2(1.0f);
				targetRectMax = targetRectMax * halfPixelSize - glm::vec2(1.0f);
				
				glUniform2f(s_spriteShader.m_targetRectMinLocation, targetRectMin.x, targetRectMin.y);
				glUniform2f(s_spriteShader.m_targetRectMaxLocation, targetRectMax.x, targetRectMax.y);
				
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
			
			position.x += glyph->advance * scale;
		}
		
		return size;
	}
}
