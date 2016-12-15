#pragma once

#include "gl/vertexarray.h"
#include "gl/bufferallocator.h"
#include "gl/shaderprogram.h"
#include "../utils/memory/stackobject.h"

#include <unordered_map>
#include <vector>

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
		static StackObject<ShaderProgram> s_shaderProgram;
		static int s_translucentUniformLocation;
		
		static GLint s_maxVertexRelativeOffset;
		static size_t s_elementsPerDrawBuffer;
		
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
			VertexArray m_vertexArray;
			
			DrawBuffer();
		};
		
		std::vector<Batch> m_materialBatches;
		
		std::vector<DrawBuffer> m_drawBuffers;
	};
}
