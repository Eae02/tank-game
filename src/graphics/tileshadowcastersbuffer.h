#pragma once

#include "gl/buffer.h"
#include "gl/vertexinputstate.h"
#include "gl/shaderprogram.h"


namespace TankGame
{
	class TileShadowCastersBuffer
	{
	public:
		TileShadowCastersBuffer(const class TileGrid& tileGrid, const class TileGridMaterial& material);
		
		static void BindShadowShader();
		
		void Draw(const struct LightInfo& lightInfo) const;
		
	private:
		static constexpr uint32_t REGION_SIZE = 16;
		
		static std::unique_ptr<ShaderProgram> s_shadowShader;
		
		struct RegionRange
		{
			uint32_t firstIndex;
			uint32_t lastIndex;
		};
		
		struct Data
		{
			std::unique_ptr<RegionRange[]> regionRanges;
			uint32_t numRegionsX;
			uint32_t numRegionsY;
			Buffer vertexBuffer;
			Buffer indexBuffer;
		};
		
		static Data BuildBuffers(const class TileGrid& tileGrid, const class TileGridMaterial& material);
		
		Data m_data;
		VertexInputState m_vertexInputState;
	};
}
