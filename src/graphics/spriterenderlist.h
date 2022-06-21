#pragma once

#include "gl/vertexinputstate.h"
#include "gl/bufferallocator.h"
#include "gl/shaderprogram.h"
#include "gl/functions.h"
#include "spritematerial.h"
#include "frames.h"
#include "../rectangle.h"

#include <unordered_map>
#include <vector>
#include <memory>
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
		
		void Add(const class Transform& transform, const SpriteMaterial& material, float z)
		{
			Add(transform, material, z, { 0, 0, 1, 1 });
		}
		
		void Add(const class Transform& transform, const SpriteMaterial& material, float z,
		         const Rectangle& textureRectangle);
		
		void End(bool isTranslucent = false);
		
	private:
		static std::unique_ptr<ShaderProgram> s_shaderProgram;
		static SpriteMaterial::UniformLocations s_spriteMaterialUniformLocations;
		static int s_translucentUniformLocation;
		
		static size_t s_elementsPerDrawBuffer;
		
		static constexpr size_t INSTANCE_MATRIX_OFF = 0;
		static constexpr size_t INSTANCE_Z_OFF = INSTANCE_MATRIX_OFF + sizeof(float) * (4 + 4 + 3);
		static constexpr size_t INSTANCE_TEX_AREA_OFF = INSTANCE_Z_OFF + sizeof(float);
		
		static constexpr size_t INSTANCE_DATA_STRIDE = INSTANCE_TEX_AREA_OFF + sizeof(float) * 4;
		
		inline static size_t GetInstanceBufferSize()
		{ return s_elementsPerDrawBuffer * INSTANCE_DATA_STRIDE * MAX_QUEUED_FRAMES; }
		
		struct InstanceData
		{
			glm::mat3 m_worldMatrix;
			float m_zIndex;
			glm::vec2 m_textureMin;
			glm::vec2 m_textureMax;
		};
		
		struct Batch
		{
			const class SpriteMaterial& m_material;
			std::vector<InstanceData> m_instances;
			
			//Assigned in End
			GLuint m_bufferOffset;
			size_t m_drawBufferIndex;
			
			inline Batch(const class SpriteMaterial& material, const InstanceData& instanceData)
			    : m_material(material), m_instances{ instanceData } { }
		};
		
		VertexInputState m_vertexInputState;
		
		std::vector<Batch> m_materialBatches;
		
		std::vector<Buffer> m_drawBuffers;
	};
}
