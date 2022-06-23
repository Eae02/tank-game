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
	static const fs::path shaderPath = fs::path("shaders") / "lighting" / "shadows";
	
	ShaderProgram ShadowRenderer::CreateBlurPassShader()
	{
		auto vs = ShaderModule::FromFile(resDirectoryPath / shaderPath / "blur.vs.glsl", GL_VERTEX_SHADER);
		auto fs = ShaderModule::FromFile(resDirectoryPath / shaderPath / "blur.fs.glsl", GL_FRAGMENT_SHADER);
		
		ShaderProgram program(vs, fs);
		program.SetTextureBinding("shadowMap", 0);
		return program;
	}
	
	ShadowRenderer::ShadowRenderer()
	    : m_blurPassShader(CreateBlurPassShader()),
	      m_inverseViewMatrixUniformLoc(m_blurPassShader.GetUniformLocation("inverseViewMatrix"))
	{
		
	}
	
	void ShadowRenderer::RenderShadowMap(ShadowMap& shadowMap, LightInfo lightInfo, const ViewInfo& viewInfo,
	                                     const ShadowRenderer::IShadowMapGeometryProvider& geometryProvider)
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
		
		if (ShadowMap::useDepthShadowMaps)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glDepthFunc(GL_GREATER);
			glEnable(GL_DEPTH_CLAMP);
		}
		
		shadowMap.BeginShadowPass(viewInfo, lightInfo);
		
		glm::mat3 invStaticViewMatrix;
		
		if (shadowMap.IsStatic())
		{
			invStaticViewMatrix = shadowMap.GetInverseStaticViewMatrix(lightInfo);
			ViewInfo staticViewInfo(shadowMap.GetStaticViewMatrix(lightInfo), &invStaticViewMatrix);
			geometryProvider.DrawShadowCasters(lightInfo, staticViewInfo);
		}
		else
		{
			geometryProvider.DrawShadowCasters(lightInfo, viewInfo);
		}
		
		if (ShadowMap::useDepthShadowMaps)
		{
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glDepthFunc(GL_LEQUAL);
			glDisable(GL_DEPTH_CLAMP);
		}
		
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
		
		QuadMesh::GetInstance().BindVAO();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		Framebuffer::Restore();
		
		lastFrameShadowMapUpdates++;
	}
	
	void ShadowRenderer::OnResize(GLsizei width, GLsizei height)
	{
		const double RESOLUTION_SCALE = 1.0;
		
		m_width = static_cast<GLsizei>(width * RESOLUTION_SCALE);
		m_height = static_cast<GLsizei>(height * RESOLUTION_SCALE);
	}
}
