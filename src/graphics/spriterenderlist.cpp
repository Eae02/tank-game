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
	StackObject<ShaderProgram> SpriteRenderList::s_shaderProgram;
	
	int SpriteRenderList::s_translucentUniformLocation;
	
	GLint SpriteRenderList::s_maxVertexRelativeOffset = -1;
	size_t SpriteRenderList::s_elementsPerDrawBuffer;
	
	static const int POSITION_ATTRIBUTE_INDEX = 0;
	static const int Z_ATTRIBUTE_INDEX = 1;
	
	SpriteRenderList::SpriteRenderList()
	{
		if (s_maxVertexRelativeOffset == -1)
		{
			glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, &s_maxVertexRelativeOffset);
			s_elementsPerDrawBuffer = s_maxVertexRelativeOffset / sizeof(glm::mat3);
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
		
		if (s_shaderProgram.IsNull())
		{
			auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "sprite.vs.glsl", GL_VERTEX_SHADER);
			auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "sprite.fs.glsl", GL_FRAGMENT_SHADER);
			
			s_shaderProgram.Construct<std::initializer_list<const ShaderModule*>>({ &vs, &fs });
			
			s_translucentUniformLocation = s_shaderProgram->GetUniformLocation("translucent");
			
			CallOnClose([] { s_shaderProgram.Destroy(); });
		}
		
		s_shaderProgram->Use();
		
		glProgramUniform1i(s_shaderProgram->GetID(), s_translucentUniformLocation, isTranslucent ? true : false);
		
		glm::mat3* matricesMemory;
		float* zValuesMemory;
		
		GLuint currentLocation = 0;
		long currentDrawBuffer = -1;
		
		for (int i = 0; i < m_materialBatches.size(); i++)
		{
			if (currentLocation > s_elementsPerDrawBuffer || currentDrawBuffer == -1)
			{
				if (currentDrawBuffer != -1)
				{
					glUnmapNamedBuffer(m_drawBuffers[currentDrawBuffer].m_matricesBuffer.GetID());
					glUnmapNamedBuffer(m_drawBuffers[currentDrawBuffer].m_zValuesBuffer.GetID());
				}
				
				currentLocation = 0;
				currentDrawBuffer++;
				
				if (currentDrawBuffer >= m_drawBuffers.size())
					m_drawBuffers.emplace_back();
				const DrawBuffer& drawBuffer = m_drawBuffers[currentDrawBuffer];
				
				matricesMemory = (glm::mat3*)glMapNamedBuffer(drawBuffer.m_matricesBuffer.GetID(), GL_WRITE_ONLY);
				zValuesMemory = (float*)glMapNamedBuffer(drawBuffer.m_zValuesBuffer.GetID(), GL_WRITE_ONLY);
			}
			
			Batch& batch = m_materialBatches[i];
			
			std::copy(batch.m_worldMatrices.begin(), batch.m_worldMatrices.end(), matricesMemory + currentLocation);
			std::copy(batch.m_spriteZ.begin(), batch.m_spriteZ.end(), zValuesMemory + currentLocation);
			
			batch.m_bufferOffset = currentLocation;
			batch.m_drawBufferIndex = currentDrawBuffer;
			currentLocation += batch.m_worldMatrices.size();
		}
		
		if (currentDrawBuffer != -1)
		{
			glUnmapNamedBuffer(m_drawBuffers[currentDrawBuffer].m_matricesBuffer.GetID());
			glUnmapNamedBuffer(m_drawBuffers[currentDrawBuffer].m_zValuesBuffer.GetID());
		}
		
		long currentVertexArray = -1;
		
		for (int i = 0; i < m_materialBatches.size(); i++)
		{
			long batchVertexArray = static_cast<long>(m_materialBatches[i].m_drawBufferIndex);
			if (currentVertexArray != batchVertexArray)
			{
				currentVertexArray = batchVertexArray;
				m_drawBuffers[batchVertexArray].m_vertexArray.Bind();
			}
			
			m_materialBatches[i].m_material.Bind();
			
			GLuint offset = m_materialBatches[i].m_bufferOffset;
			glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, offset * sizeof(float));
			for (GLuint l = 0; l < 3; l++)
				glVertexAttribFormat(2 + l, 3, GL_FLOAT, GL_FALSE, offset * sizeof(glm::mat3));
			
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_materialBatches[i].m_worldMatrices.size());
		}
	}
	
	SpriteRenderList::DrawBuffer::DrawBuffer()
	    : m_matricesBuffer(s_elementsPerDrawBuffer * sizeof(glm::mat3), GL_MAP_WRITE_BIT),
	      m_zValuesBuffer(s_elementsPerDrawBuffer * sizeof(float), GL_MAP_WRITE_BIT)
	{
		for (GLuint i = 0; i < 5; i++)
			glEnableVertexArrayAttrib(m_vertexArray.GetID(), i);
		
		glVertexArrayVertexBuffer(m_vertexArray.GetID(), POSITION_ATTRIBUTE_INDEX,
		                          QuadMesh::GetInstance().GetVBO(), 0, sizeof(float) * 2);
		glVertexArrayAttribFormat(m_vertexArray.GetID(), POSITION_ATTRIBUTE_INDEX, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(m_vertexArray.GetID(), POSITION_ATTRIBUTE_INDEX, POSITION_ATTRIBUTE_INDEX);
		
		glVertexArrayBindingDivisor(m_vertexArray.GetID(), Z_ATTRIBUTE_INDEX, 1);
		glVertexArrayVertexBuffer(m_vertexArray.GetID(), Z_ATTRIBUTE_INDEX, m_zValuesBuffer.GetID(), 0, sizeof(float));
		glVertexArrayAttribFormat(m_vertexArray.GetID(), Z_ATTRIBUTE_INDEX, 1, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(m_vertexArray.GetID(), Z_ATTRIBUTE_INDEX, Z_ATTRIBUTE_INDEX);
		
		for (GLuint i = 0; i < 3; i++)
		{
			GLuint location = 2 + i;
			
			glVertexArrayBindingDivisor(m_vertexArray.GetID(), location, 1);
			glVertexArrayVertexBuffer(m_vertexArray.GetID(), location, m_matricesBuffer.GetID(),
			                          sizeof(float) * 3 * i, sizeof(glm::mat3));
			glVertexArrayAttribFormat(m_vertexArray.GetID(), location, 3, GL_FLOAT, GL_FALSE, 0);
			glVertexArrayAttribBinding(m_vertexArray.GetID(), location, location);
		}
	}
	
}
