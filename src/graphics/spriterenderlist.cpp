#include "spriterenderlist.h"
#include "spritematerial.h"
#include "gl/shadermodule.h"
#include "quadmesh.h"
#include "../utils/ioutils.h"
#include "../transform.h"
#include "../utils/utils.h"


namespace TankGame
{
	std::unique_ptr<ShaderProgram> SpriteRenderList::s_shaderProgram;
	SpriteMaterial::UniformLocations SpriteRenderList::s_spriteMaterialUniformLocations;
	int SpriteRenderList::s_translucentUniformLocation;
	
	size_t SpriteRenderList::s_elementsPerDrawBuffer = 1024;
	
	SpriteRenderList::SpriteRenderList()
	{
		QuadMesh::GetInstance().InitializeVertexAttribute(m_vertexInputState, 0);
		for (uint32_t i = 1; i <= 5; i++)
			m_vertexInputState.SetAttributeInstanceStep(i);
	}
	
	void SpriteRenderList::Begin()
	{
		m_materialBatches.clear();
	}
	
	void SpriteRenderList::Add(const Transform& transform, const SpriteMaterial& material, float z,
	                           const Rectangle& textureRectangle)
	{
#ifndef NDEBUG
		if (z < 0 || z > 1)
			Panic("Z out of range.");
#endif
		
		auto batchPos = std::find_if(m_materialBatches.begin(), m_materialBatches.end(), [&] (const Batch& batch)
		{
			return &batch.m_material == &material;
		});
		
		const InstanceData instanceData =
		{
			/* m_worldMatrix */ transform.GetMatrix(),
			/* m_zIndex      */ z,
			/* m_textureMin  */ textureRectangle.NearPos(),
			/* m_textureMax  */ textureRectangle.FarPos()
		};
		
		if (batchPos == m_materialBatches.end())
			m_materialBatches.emplace_back(material, instanceData);
		else
			batchPos->m_instances.push_back(instanceData);
	}
	
	void SpriteRenderList::End(bool isTranslucent)
	{
		if (Empty())
			return;
		
		size_t drawBufferOffset = s_elementsPerDrawBuffer * GetFrameQueueIndex();
		
		//Creates the sprite shader if it hasn't been created already
		if (s_shaderProgram == nullptr)
		{
			s_shaderProgram = std::make_unique<ShaderProgram>(
				ShaderModule::FromResFile("sprite.vs.glsl"), ShaderModule::FromResFile("sprite.fs.glsl"));
			
			s_shaderProgram->SetTextureBinding("diffuseSampler", SpriteMaterial::DIFFUSE_TEXTURE_UNIT);
			s_shaderProgram->SetTextureBinding("normalMapSampler", SpriteMaterial::NORMAL_MAP_TEXTURE_UNIT);
			
			s_spriteMaterialUniformLocations.shadeLoc = s_shaderProgram->GetUniformLocation("shade");
			s_spriteMaterialUniformLocations.specularIntensityLoc = s_shaderProgram->GetUniformLocation("specularIntensity");
			s_spriteMaterialUniformLocations.specularExponentLoc = s_shaderProgram->GetUniformLocation("specularExponent");
			s_translucentUniformLocation = s_shaderProgram->GetUniformLocation("translucent");
			
			CallOnClose([] { s_shaderProgram = nullptr; });
		}
		
		s_shaderProgram->Use();
		
		glUniform1i(s_translucentUniformLocation, isTranslucent ? 1 : 0);
		
		// ** Prepares instance data buffers **
		
		//Stores the index in m_drawBuffers for the currently mapped instance data buffer.
		size_t currentDrawBuffer = 0;
		
		//Stores a pointer to persistently mapped memory for the current instance data buffer.
		char* instanceDataMemory = nullptr;
		
		//Stores the next offset (in instances) to write to in the current instance data buffer.
		GLuint currentBufferOffset = 0;
		
		for (size_t i = 0; i < m_materialBatches.size(); i++)
		{
			if (instanceDataMemory == nullptr || currentBufferOffset > s_elementsPerDrawBuffer)
			{
				//Moves to the next instance data buffer and resets the offset
				if (instanceDataMemory != nullptr)
				{
					currentBufferOffset = 0;
					currentDrawBuffer++;
				}
				
				//Creates more buffers as neccesary
				if (currentDrawBuffer >= m_drawBuffers.size())
					m_drawBuffers.emplace_back(GetInstanceBufferSize(), BufferUsage::MapWritePersistentUBO);
				
				instanceDataMemory = m_drawBuffers[currentDrawBuffer].MappedMemory() +
				                     drawBufferOffset * INSTANCE_DATA_STRIDE;
			}
			
			Batch& batch = m_materialBatches[i];
			
			batch.m_bufferOffset = currentBufferOffset;
			batch.m_drawBufferIndex = currentDrawBuffer;
			
			//Copies instance data to the instance data buffer
			for (const InstanceData& instanceData : batch.m_instances)
			{
				float* instanceDataOut = reinterpret_cast<float*>(instanceDataMemory +
				                                                  currentBufferOffset * INSTANCE_DATA_STRIDE);
				
				//Copies the world matrix
				for (int c = 0; c < 3; c++)
				{
					for (int r = 0; r < 3; r++)
						instanceDataOut[c * 4 + r] = instanceData.m_worldMatrix[c][r];
				}
				
				//Copies the z value
				instanceDataOut[INSTANCE_Z_OFF / sizeof(float)] = instanceData.m_zIndex;
				
				//Copies the texture area
				instanceDataOut[INSTANCE_TEX_AREA_OFF / sizeof(float) + 0] = instanceData.m_textureMin.x;
				instanceDataOut[INSTANCE_TEX_AREA_OFF / sizeof(float) + 1] = instanceData.m_textureMin.y;
				instanceDataOut[INSTANCE_TEX_AREA_OFF / sizeof(float) + 2] = instanceData.m_textureMax.x;
				instanceDataOut[INSTANCE_TEX_AREA_OFF / sizeof(float) + 3] = instanceData.m_textureMax.y;
				
				currentBufferOffset++;
			}
		}
		
		// ** Renders sprites **
		
		m_vertexInputState.Bind();
		
		for (size_t i = 0; i <= currentDrawBuffer; i++)
		{
			const GLuint usedLength = i == currentDrawBuffer ? currentBufferOffset : s_elementsPerDrawBuffer;
			
			m_drawBuffers[i].FlushMappedMemory(drawBufferOffset * INSTANCE_DATA_STRIDE, usedLength * INSTANCE_DATA_STRIDE);
			
			std::array<GLuint, 5> instanceBuffers;
			std::fill_n(instanceBuffers.data(), instanceBuffers.size(), m_drawBuffers[i].GetID());
			
			for (const Batch& materialBatch : m_materialBatches)
			{
				if (materialBatch.m_drawBufferIndex != i)
					continue;
				
				size_t instanceByteOffset = (materialBatch.m_bufferOffset + drawBufferOffset) * INSTANCE_DATA_STRIDE;
				
				const GLintptr instanceOffsets[5] =
				{
					static_cast<GLintptr>(instanceByteOffset + INSTANCE_Z_OFF),
					static_cast<GLintptr>(instanceByteOffset + INSTANCE_TEX_AREA_OFF),
					static_cast<GLintptr>(instanceByteOffset + INSTANCE_MATRIX_OFF + sizeof(float) * 4 * 0),
					static_cast<GLintptr>(instanceByteOffset + INSTANCE_MATRIX_OFF + sizeof(float) * 4 * 1),
					static_cast<GLintptr>(instanceByteOffset + INSTANCE_MATRIX_OFF + sizeof(float) * 4 * 2)
				};
				static const VertexAttribFormat vertexAttribFormats[5] = 
				{
					VertexAttribFormat::Float32_1,
					VertexAttribFormat::Float32_4,
					VertexAttribFormat::Float32_3,
					VertexAttribFormat::Float32_3,
					VertexAttribFormat::Float32_3,
				};
				for (size_t b = 0; b < 5; b++)
				{
					m_vertexInputState.UpdateAttribute(b + 1, m_drawBuffers[i].GetID(), vertexAttribFormats[b],
					                                   instanceOffsets[b], INSTANCE_DATA_STRIDE);
				}
				
				materialBatch.m_material.Bind(s_spriteMaterialUniformLocations);
				
				glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, materialBatch.m_instances.size());
			}
		}
	}
}
