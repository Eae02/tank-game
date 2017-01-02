#include "uirenderer.h"
#include "font.h"
#include "../gl/shadermodule.h"
#include "../quadmesh.h"
#include "../../utils/utils.h"
#include "../../utils/mathutils.h"
#include "../../utils/ioutils.h"

namespace TankGame
{
	std::unique_ptr<UIRenderer> UIRenderer::s_instance;
	
	UIRenderer::SpriteShader UIRenderer::s_spriteShader;
	UIRenderer::TextShader UIRenderer::s_textShader;
	
	UIRenderer::QuadShader UIRenderer::s_quadShader;
	UIRenderer::LineShader UIRenderer::s_lineShader;
	
	void UIRenderer::LoadSpriteShader()
	{
		auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "ui" / "uisprite.vs.glsl", GL_VERTEX_SHADER);
		auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "ui" / "uisprite.fs.glsl", GL_FRAGMENT_SHADER);
		
		s_spriteShader.m_shader.reset(new ShaderProgram{ &vs, &fs });
		CallOnClose([] { s_spriteShader.m_shader = nullptr; });
		
		s_spriteShader.m_sampleRectMinLocation = s_spriteShader.m_shader->GetUniformLocation("sampleRectMin");
		s_spriteShader.m_sampleRectMaxLocation = s_spriteShader.m_shader->GetUniformLocation("sampleRectMax");
		s_spriteShader.m_targetRectMinLocation = s_spriteShader.m_shader->GetUniformLocation("targetRectMin");
		s_spriteShader.m_targetRectMaxLocation = s_spriteShader.m_shader->GetUniformLocation("targetRectMax");
		s_spriteShader.m_shadeLocation = s_spriteShader.m_shader->GetUniformLocation("shade");
	}
	
	void UIRenderer::LoadQuadShader()
	{
		auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "ui" / "uiquad.vs.glsl", GL_VERTEX_SHADER);
		auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "ui" / "uiquad.fs.glsl", GL_FRAGMENT_SHADER);
		
		s_quadShader.m_shader.reset(new ShaderProgram{ &vs, &fs });
		CallOnClose([] { s_quadShader.m_shader = nullptr; });
		
		s_quadShader.m_colorLocation = s_quadShader.m_shader->GetUniformLocation("color");
		s_quadShader.m_transformLocation = s_quadShader.m_shader->GetUniformLocation("transform");
	}
	
	void UIRenderer::LoadLineShader()
	{
		auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "ui" / "uiline.vs.glsl", GL_VERTEX_SHADER);
		auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "ui" / "uiquad.fs.glsl", GL_FRAGMENT_SHADER);
		
		s_lineShader.m_shader.reset(new ShaderProgram{ &vs, &fs });
		CallOnClose([] { s_lineShader.m_shader = nullptr; });
		
		s_lineShader.m_vertex1Location = s_lineShader.m_shader->GetUniformLocation("vertices[0]");
		s_lineShader.m_vertex2Location = s_lineShader.m_shader->GetUniformLocation("vertices[1]");
		s_lineShader.m_colorLocation = s_lineShader.m_shader->GetUniformLocation("color");
	}
	
	void UIRenderer::LoadTextShader()
	{
		auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "ui" / "text.vs.glsl", GL_VERTEX_SHADER);
		auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "ui" / "text.fs.glsl", GL_FRAGMENT_SHADER);
		
		s_textShader.m_shader.reset(new ShaderProgram{ &vs, &fs });
		CallOnClose([] { s_textShader.m_shader = nullptr; });
		
		s_textShader.m_offsetLocation = s_textShader.m_shader->GetUniformLocation("offset");
		s_textShader.m_sizeLocation = s_textShader.m_shader->GetUniformLocation("size");
		s_textShader.m_colorLocation = s_textShader.m_shader->GetUniformLocation("color");
	}
	
	void UIRenderer::DrawSprite(const Texture2D& texture, const Rectangle& targetRectangle,
	                            const Rectangle& sampleRectangle, const glm::vec4& shade) const
	{
		if (s_spriteShader.m_shader== nullptr)
			LoadSpriteShader();
		s_spriteShader.m_shader->Use();
		
		texture.Bind(0);
		
		GLuint shaderID = s_spriteShader.m_shader->GetID();
		
		glProgramUniform2f(shaderID, s_spriteShader.m_sampleRectMinLocation,
		                   sampleRectangle.x / texture.GetWidth(), sampleRectangle.y / texture.GetHeight());
		glProgramUniform2f(shaderID, s_spriteShader.m_sampleRectMaxLocation,
		                   sampleRectangle.FarX() / texture.GetWidth(), sampleRectangle.FarY() / texture.GetHeight());
		
		glm::vec2 targetRectMin(targetRectangle.x / m_windowWidth, targetRectangle.y / m_windowHeight);
		glm::vec2 targetRectMax(targetRectangle.FarX() / m_windowWidth, targetRectangle.FarY() / m_windowHeight);
		
		targetRectMin = targetRectMin * 2.0f - glm::vec2(1.0f);
		targetRectMax = targetRectMax * 2.0f - glm::vec2(1.0f);
		
		glProgramUniform2f(shaderID, s_spriteShader.m_targetRectMinLocation, targetRectMin.x, targetRectMin.y);
		glProgramUniform2f(shaderID, s_spriteShader.m_targetRectMaxLocation, targetRectMax.x, targetRectMax.y);
		
		glProgramUniform4fv(shaderID, s_spriteShader.m_shadeLocation, 1, reinterpret_cast<const GLfloat*>(&shade));
		
		QuadMesh::GetInstance().GetVAO().Bind();
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
		DrawQuad(MapNDCToRectangle(rectangle), color);
	}
	
	void UIRenderer::DrawQuad(const glm::mat3& transform, const glm::vec4& color) const
	{
		if (s_quadShader.m_shader== nullptr)
			LoadQuadShader();
		s_quadShader.m_shader->Use();
		
		glm::mat3 fullTransform = glm::transpose(glm::mat3(
				2.0f / m_windowWidth, 0.0f, -1.0f,
				0.0f, 2.0f / m_windowHeight, -1.0f,
				0.0f, 0.0f, 1.0f
		)) * transform;
		
		glProgramUniformMatrix3fv(s_quadShader.m_shader->GetID(), s_quadShader.m_transformLocation, 1, GL_FALSE,
		                          reinterpret_cast<const GLfloat*>(&fullTransform));
		
		glProgramUniform4fv(s_quadShader.m_shader->GetID(), s_quadShader.m_colorLocation, 1,
		                    reinterpret_cast<const GLfloat*>(&color));
		
		QuadMesh::GetInstance().GetVAO().Bind();
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
		
		glProgramUniform2f(s_lineShader.m_shader->GetID(), s_lineShader.m_vertex1Location, vertex1.x, vertex1.y);
		glProgramUniform2f(s_lineShader.m_shader->GetID(), s_lineShader.m_vertex2Location, vertex2.x, vertex2.y);
		
		glProgramUniform4fv(s_lineShader.m_shader->GetID(), s_lineShader.m_colorLocation, 1,
		                    reinterpret_cast<const GLfloat*>(&color));
		
		m_lineVAO.Bind();
		glDrawArrays(GL_LINES, 0, 2);
	}
	
	glm::vec2 UIRenderer::DrawString(const class Font& font, const std::string& string, Rectangle rectangle,
	                                 Alignment alignX, Alignment alignY, const glm::vec4& color, float scale) const
	{
		return DrawString(font, UTF8ToUTF32(string), rectangle, alignX, alignY, color, scale);
	}
	
	glm::vec2 UIRenderer::DrawString(const Font& font, const std::u32string& string, Rectangle rectangle,
	                                 Alignment alignX, Alignment alignY, const glm::vec4& color, float scale) const
	{
		if (s_textShader.m_shader== nullptr)
			LoadTextShader();
		s_textShader.m_shader->Use();
		
		glm::vec2 size = font.MeasureString(string) * scale;
		
		glm::vec2 position;
		position.x = rectangle.x + (static_cast<int>(alignX) / 2.0f) * (rectangle.w - size.x);
		position.y = rectangle.y + (static_cast<int>(alignY) / 2.0f) * (rectangle.h - size.y);
		
		glm::vec2 halfScreenSize(m_windowWidth / 2.0f, m_windowHeight / 2.0f);
		
		GLuint shaderID = s_textShader.m_shader->GetID();
		glProgramUniform4fv(shaderID, s_textShader.m_colorLocation, 1, reinterpret_cast<const GLfloat*>(&color));
		
		QuadMesh::GetInstance().GetVAO().Bind();
		
		for (size_t i = 0; i < string.length(); i++)
		{
			const Font::Glyph* glyph = font.TryGetGlyph(string[i]);
			
			if (glyph == nullptr)
				continue;
			
			if (glyph->m_texture != nullptr)
			{
				glyph->m_texture->Bind(0);
				
				glm::vec2 bearingOffset(glyph->m_bearing.x, glyph->m_bearing.y - glyph->m_texture->GetHeight());
				glm::vec2 offset = (glm::floor(position + bearingOffset * scale) / halfScreenSize) - glm::vec2(1.0f);
				glm::vec2 size =
					(glm::vec2(glyph->m_texture->GetWidth(), glyph->m_texture->GetHeight()) * scale) / halfScreenSize;
				
				glProgramUniform2f(shaderID, s_textShader.m_offsetLocation, offset.x, offset.y);
				glProgramUniform2f(shaderID, s_textShader.m_sizeLocation, size.x, size.y);
				
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
			
			position.x += glyph->m_advance * scale;
		}
		
		return size;
	}
}
