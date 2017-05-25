#include "spriterenderlist.h"
#include "spritematerial.h"
#include "gl/shadermodule.h"
#include "quadmesh.h"
#include "../utils/ioutils.h"
#include "../transform.h"
#include "../utils/utils.h"

#include <algorithm>

namespace TankGame
{
	std::unique_ptr<ShaderProgram> SpriteRenderList::s_shaderProgram;
	
	int SpriteRenderList::s_translucentUniformLocation;
	
	size_t SpriteRenderList::s_elementsPerDrawBuffer = 1024;
	
	static const int POSITION_ATTRIBUTE_INDEX = 0;
	static const int Z_ATTRIBUTE_INDEX = 1;
	
	SpriteRenderList::SpriteRenderList()
	{
		for (GLuint i = 0; i < 5; i++)
			glEnableVertexArrayAttrib(m_vertexArray.GetID(), i);
		
		glVertexArrayVertexBuffer(m_vertexArray.GetID(), POSITION_ATTRIBUTE_INDEX,
		                          QuadMesh::GetInstance().GetVBO(), 0, sizeof(float) * 2);
		glVertexArrayAttribFormat(m_vertexArray.GetID(), POSITION_ATTRIBUTE_INDEX, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(m_vertexArray.GetID(), POSITION_ATTRIBUTE_INDEX, POSITION_ATTRIBUTE_INDEX);
		
		glVertexArrayBindingDivisor(m_vertexArray.GetID(), Z_ATTRIBUTE_INDEX, 1);
		glVertexArrayAttribFormat(m_vertexArray.GetID(), Z_ATTRIBUTE_INDEX, 1, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(m_vertexArray.GetID(), Z_ATTRIBUTE_INDEX, Z_ATTRIBUTE_INDEX);
		
		for (GLuint i = 0; i < 3; i++)
		{
			GLuint location = 2 + i;
			
			glVertexArrayBindingDivisor(m_vertexArray.GetID(), location, 1);
			glVertexArrayAttribFormat(m_vertexArray.GetID(), location, 3, GL_FLOAT, GL_FALSE, 0);
			glVertexArrayAttribBinding(m_vertexArray.GetID(), location, location);
		}
	}
	
	void SpriteRenderList::Begin()
	{
		m_materialBatches.clear();
	}
	
	void SpriteRenderList::Add(const Transform& transform, const SpriteMaterial& material, float z)
	{
		if (z < 0 || z > 1)
			throw std::runtime_error("Z out of range.");
		
		auto batchPos = std::find_if(m_materialBatches.begin(), m_materialBatches.end(), [&] (const Batch& batch)
		{
			return &batch.m_material == &material;
		});
		
		if (batchPos == m_materialBatches.end())
			m_materialBatches.emplace_back(material, transform.GetMatrix(), z);
		else
		{
			batchPos->m_worldMatrices.push_back(transform.GetMatrix());
			batchPos->m_spriteZ.push_back(z);
		}
	}
	
	void SpriteRenderList::End(bool isTranslucent)
	{
		if (Empty())
			return;
		
		size_t drawBufferOffset = s_elementsPerDrawBuffer * GetFrameQueueIndex();
		
		//Creates the sprite shader if it hasn't been created already
		if (s_shaderProgram == nullptr)
		{
			auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "sprite.vs.glsl", GL_VERTEX_SHADER);
			auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "sprite.fs.glsl", GL_FRAGMENT_SHADER);
			
			s_shaderProgram.reset(new ShaderProgram{ &vs, &fs });
			
			s_translucentUniformLocation = s_shaderProgram->GetUniformLocation("translucent");
			
			CallOnClose([] { s_shaderProgram = nullptr; });
		}
		
		s_shaderProgram->Use();
		
		glUniform1i(s_translucentUniformLocation, isTranslucent ? 1 : 0);
		
		glm::mat3x4* matricesMemory;
		float* zValuesMemory;
		
		GLuint currentBufferOffset = 0;
		long currentDrawBuffer = -1;
		
		for (size_t i = 0; i < m_materialBatches.size(); i++)
		{
			if (currentBufferOffset > s_elementsPerDrawBuffer || currentDrawBuffer == -1)
			{
				currentBufferOffset = 0;
				currentDrawBuffer++;
				
				if (currentDrawBuffer >= static_cast<long>(m_drawBuffers.size()))
					m_drawBuffers.emplace_back();
				const DrawBuffer& drawBuffer = m_drawBuffers[currentDrawBuffer];
				
				matricesMemory = reinterpret_cast<glm::mat3x4*>(drawBuffer.m_matricesMemory) + drawBufferOffset;
				zValuesMemory = reinterpret_cast<float*>(drawBuffer.m_zValuesMemory) + drawBufferOffset;
			}
			
			Batch& batch = m_materialBatches[i];
			
			std::transform(batch.m_worldMatrices.begin(), batch.m_worldMatrices.end(),
			               matricesMemory + currentBufferOffset, [] (const glm::mat3& in)
			{
				return glm::mat3x4(in);
			});
			
			std::copy(batch.m_spriteZ.begin(), batch.m_spriteZ.end(), zValuesMemory + currentBufferOffset);
			
			batch.m_bufferOffset = currentBufferOffset;
			batch.m_drawBufferIndex = currentDrawBuffer;
			currentBufferOffset += batch.m_worldMatrices.size();
		}
		
		m_vertexArray.Bind();
		
		const size_t matrixStride = sizeof(float) * 4 * 3;
		
		for (long i = 0; i <= currentDrawBuffer; i++)
		{
			GLuint usedLength = i == currentDrawBuffer ? currentBufferOffset : s_elementsPerDrawBuffer;
			
			glFlushMappedNamedBufferRange(m_drawBuffers[i].m_matricesBuffer.GetID(),
			                              drawBufferOffset * matrixStride, usedLength * matrixStride);
			
			glFlushMappedNamedBufferRange(m_drawBuffers[i].m_zValuesBuffer.GetID(),
			                              drawBufferOffset * sizeof(float), usedLength * sizeof(float));
			
			GLuint matricesBuffer = m_drawBuffers[i].m_matricesBuffer.GetID();
			GLuint zValuesBuffer = m_drawBuffers[i].m_zValuesBuffer.GetID();
			
			for (const Batch& materialBatch : m_materialBatches)
			{
				if (static_cast<long>(materialBatch.m_drawBufferIndex) != i)
					continue;
				
				size_t offset = materialBatch.m_bufferOffset + drawBufferOffset;
				
				const GLuint buffers[] = { zValuesBuffer, matricesBuffer, matricesBuffer, matricesBuffer };
				const GLintptr offsets[] =
				{
					static_cast<GLintptr>(offset * sizeof(float)),
					static_cast<GLintptr>(offset * matrixStride + sizeof(float) * 4 * 0),
					static_cast<GLintptr>(offset * matrixStride + sizeof(float) * 4 * 1),
					static_cast<GLintptr>(offset * matrixStride + sizeof(float) * 4 * 2)
				};
				const GLsizei strides[] = { sizeof(float), matrixStride, matrixStride, matrixStride };
				
				glBindVertexBuffers(Z_ATTRIBUTE_INDEX, 4, buffers, offsets, strides);
				
				materialBatch.m_material.Bind();
				
				glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, materialBatch.m_worldMatrices.size());
			}
		}
	}
	
	SpriteRenderList::DrawBuffer::DrawBuffer()
	    : m_matricesBuffer(GetMatricesBufferSize(), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT),
	      m_zValuesBuffer(GetZValuesBufferSize(), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT),
	      m_matricesMemory(glMapNamedBufferRange(m_matricesBuffer.GetID(), 0, GetMatricesBufferSize(),
	                                             GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT)),
	      m_zValuesMemory(glMapNamedBufferRange(m_zValuesBuffer.GetID(), 0, GetZValuesBufferSize(),
	                                            GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT))
	{
		
	}
	
}
