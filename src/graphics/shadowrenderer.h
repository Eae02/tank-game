#pragma once

#include "gl/shadermodule.h"
#include "gl/shaderprogram.h"
#include "../utils/abstract.h"
#include "../world/lights/ilightsource.h"

#include <glm/glm.hpp>
#include <memory>

namespace TankGame
{
	class ShadowRenderer
	{
	public:
		class IShadowMapGeometryProvider : public Abstract
		{
		public:
			virtual void DrawShadowCasters(glm::vec2 lightPos, const class ViewInfo& viewInfo) const = 0;
		};
		
		ShadowRenderer();
		
		void RenderShadowMap(class ShadowMap& shadowMap, LightInfo lightInfo, const class ViewInfo& viewInfo,
		                     const IShadowMapGeometryProvider& geometryProvider) const;
		
		void OnResize(GLsizei width, GLsizei height);
		
		static const ShaderModule& GetFragmentShader();
		
	private:
		static std::unique_ptr<ShaderModule> s_fragmentShader;
		
		static ShaderProgram CreateBlurPassShader();
		
		ShaderProgram m_blurPassShader;
		int m_inverseViewMatrixUniformLoc;
		
		GLsizei m_width = 0;
		GLsizei m_height = 0;
	};
}
