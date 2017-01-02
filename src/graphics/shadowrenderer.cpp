#include "shadowrenderer.h"
#include "shadowmap.h"
#include "quadmesh.h"
#include "../world/gameworld.h"
#include "../world/tilegrid.h"
#include "../utils/ioutils.h"
#include "../utils/utils.h"

#include <iostream>

namespace TankGame
{
	std::unique_ptr<ShaderModule> ShadowRenderer::s_geometryShader;
	std::unique_ptr<ShaderModule> ShadowRenderer::s_fragmentShader;
	
	static const fs::path shaderPath = fs::path("shaders") / "lighting" / "shadows";
	
	const ShaderModule& ShadowRenderer::GetGeometryShader()
	{
		if (s_geometryShader== nullptr)
		{
			const fs::path gsPath = GetResDirectory() / shaderPath / "shadow.gs.glsl";
			s_geometryShader = std::make_unique<ShaderModule>(ShaderModule::FromFile(gsPath, GL_GEOMETRY_SHADER));
			CallOnClose([] { s_geometryShader = nullptr; });
		}
		
		return *s_geometryShader;
	}
	
	const ShaderModule& ShadowRenderer::GetFragmentShader()
	{
		if (s_fragmentShader== nullptr)
		{
			const fs::path fsPath = GetResDirectory() / shaderPath / "shadow.fs.glsl";
			s_fragmentShader = std::make_unique<ShaderModule>(ShaderModule::FromFile(fsPath, GL_FRAGMENT_SHADER));
			CallOnClose([] { s_fragmentShader = nullptr; });
		}
		
		return *s_fragmentShader;
	}
	
	ShaderProgram ShadowRenderer::CreateBlurPassShader()
	{
		auto vs = ShaderModule::FromFile(GetResDirectory() / shaderPath / "blur.vs.glsl", GL_VERTEX_SHADER);
		auto fs = ShaderModule::FromFile(GetResDirectory() / shaderPath / "blur.fs.glsl", GL_FRAGMENT_SHADER);
		
		return ShaderProgram({ &vs, &fs });
	}
	
	ShadowRenderer::ShadowRenderer()
	    : m_blurPassShader(CreateBlurPassShader()),
	      m_inverseViewMatrixUniformLoc(m_blurPassShader.GetUniformLocation("inverseViewMatrix"))
	{
		
	}
	
	void ShadowRenderer::RenderShadowMap(ShadowMap& shadowMap, LightInfo lightInfo, const ViewInfo& viewInfo,
	                                     const ShadowRenderer::IShadowMapGeometryProvider& geometryProvider) const
	{
		const float STATIC_SM_RESOLUTION_PER_UNIT = 16;
		
		glm::tvec2<GLsizei, glm::highp> resolution = [&] {
			if (!shadowMap.IsStatic())
				return glm::tvec2<GLsizei, glm::highp>(m_width, m_height);
			
			float resPerUnit = STATIC_SM_RESOLUTION_PER_UNIT * shadowMap.GetStaticQualityMultiplier();
			float res = std::ceil(resPerUnit * lightInfo.m_range * 2);
			return glm::tvec2<GLsizei, glm::highp>(static_cast<int>(res), static_cast<int>(res));
		}();
		
		if (resolution.x < 64)
			resolution.x = 64;
		if (resolution.y < 64)
			resolution.y = 64;
		
		if (shadowMap.GetWidth() >= resolution.x && shadowMap.GetHeight() >= resolution.y && !shadowMap.NeedsRedraw())
			return;
		
		shadowMap.SetResolution(resolution.x, resolution.y);
		shadowMap.ClearNeedsRedraw();
		
		Framebuffer::Save();
		shadowMap.BeginShadowPass(viewInfo, lightInfo);
		
		glDisable(GL_DEPTH_TEST);
		
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ZERO);
		glBlendEquation(GL_FUNC_ADD);
		
		glm::mat3 invStaticViewMatrix;
		
		if (shadowMap.IsStatic())
		{
			invStaticViewMatrix = shadowMap.GetInverseStaticViewMatrix(lightInfo);
			ViewInfo staticViewInfo(shadowMap.GetStaticViewMatrix(lightInfo), &invStaticViewMatrix);
			geometryProvider.DrawShadowCasters(lightInfo.m_position, staticViewInfo);
		}
		else
			geometryProvider.DrawShadowCasters(lightInfo.m_position, viewInfo);
		
		glDisable(GL_BLEND);
		
		shadowMap.BeginBlurPass();
		
		m_blurPassShader.Use();
		
		if (shadowMap.IsStatic())
		{
			glProgramUniformMatrix3fv(m_blurPassShader.GetID(), m_inverseViewMatrixUniformLoc, 1, GL_FALSE,
			                          reinterpret_cast<const GLfloat*>(&invStaticViewMatrix));
		}
		else
		{
			glProgramUniformMatrix3fv(m_blurPassShader.GetID(), m_inverseViewMatrixUniformLoc, 1, GL_FALSE,
			                          reinterpret_cast<const GLfloat*>(&viewInfo.GetInverseViewMatrix()));
		}
		
		QuadMesh::GetInstance().GetVAO().Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		glEnable(GL_DEPTH_TEST);
		
		Framebuffer::Restore();
	}
	
	void ShadowRenderer::OnResize(GLsizei width, GLsizei height)
	{
		const double RESOLUTION_SCALE = 1.0;
		
		m_width = static_cast<GLsizei>(width * RESOLUTION_SCALE);
		m_height = static_cast<GLsizei>(height * RESOLUTION_SCALE);
	}
}
