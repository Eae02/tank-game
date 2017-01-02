#pragma once

#include "gl/buffer.h"
#include "gl/vertexarray.h"
#include "gl/shaderprogram.h"

#include <memory>

namespace TankGame
{
	class TileShadowCastersBuffer
	{
	public:
		TileShadowCastersBuffer(const class TileGrid& tileGrid, const class TileGridMaterial& material);
		
		static void BindShadowShader();
		
		void Draw() const;
		
	private:
		static std::unique_ptr<ShaderProgram> s_shadowShader;
		
		GLuint m_numVertices;
		
		Buffer m_buffer;
		VertexArray m_vertexArray;
	};
}
