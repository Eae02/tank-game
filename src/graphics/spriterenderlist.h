#pragma once

#include "gl/vertexarray.h"
#include "gl/bufferallocator.h"
#include "gl/shaderprogram.h"

#include <unordered_map>
#include <vector>
#include <memory>

#include "frames.h"
#include "gl/functions.h"
#include <glm/glm.hpp>

namespace TankGame
{
	class SpriteRenderList
	{
	public:
		SpriteRenderList();
		
		inline bool Empty() const
		{ return m_materialBatches.empty(); }
		
		void Begin();
		void Add(const class Transform& transform, const class SpriteMaterial& material, float z);
		void End(bool isTranslucent = false);
		
	private:
		static std::unique_ptr<ShaderProgram> s_shaderProgram;
		static int s_translucentUniformLocation;
		
		static size_t s_elementsPerDrawBuffer;
		
		inline static size_t GetMatricesBufferSize()
		{ return s_elementsPerDrawBuffer * sizeof(float) * 4 * 3 * MAX_QUEUED_FRAMES; }
		inline static size_t GetZValuesBufferSize()
		{ return s_elementsPerDrawBuffer * sizeof(float) * MAX_QUEUED_FRAMES; }
		
		struct Batch
		{
			const class SpriteMaterial& m_material;
			std::vector<glm::mat3> m_worldMatrices;
			std::vector<float> m_spriteZ;
			GLuint m_bufferOffset;
			size_t m_drawBufferIndex;
			
			inline Batch(const class SpriteMaterial& material, const glm::mat3& worldMatrix, float z)
			    : m_material(material), m_worldMatrices{ worldMatrix }, m_spriteZ{ z } { }
		};
		
		struct DrawBuffer
		{
			Buffer m_matricesBuffer;
			Buffer m_zValuesBuffer;
			
			void* m_matricesMemory;
			void* m_zValuesMemory;
			
			DrawBuffer();
		};
		
		VertexArray m_vertexArray;
		
		std::vector<Batch> m_materialBatches;
		
		std::vector<DrawBuffer> m_drawBuffers;
	};
}
