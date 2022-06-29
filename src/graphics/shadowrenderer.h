#pragma once

#include "gl/shadermodule.h"
#include "gl/shaderprogram.h"
#include "../utils/abstract.h"
#include "../world/lights/ilightsource.h"


namespace TankGame
{
	class ShadowRenderer
	{
	public:
		class IShadowMapGeometryProvider : public Abstract
		{
		public:
			virtual void DrawShadowCasters(const LightInfo& lightInfo, const class ViewInfo& viewInfo) const = 0;
		};
		
		ShadowRenderer();
		
		void RenderShadowMap(class ShadowMap& shadowMap, LightInfo lightInfo, const class ViewInfo& viewInfo,
		                     const IShadowMapGeometryProvider& geometryProvider);
		
		void OnResize(GLsizei width, GLsizei height);
		
		uint32_t lastFrameShadowMapUpdates = 0;
		
	private:
		static ShaderProgram CreateBlurPassShader();
		
		ShaderProgram m_blurPassShader;
		int m_inverseViewMatrixUniformLoc;
		
		GLsizei m_width = 0;
		GLsizei m_height = 0;
	};
}
