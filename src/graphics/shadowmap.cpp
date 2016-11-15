#include <cstring>
#include "shadowmap.h"
#include "../utils/utils.h"
#include "../utils/mathutils.h"

namespace TankGame
{
	StackObject<Texture2D> ShadowMap::s_defaultTexture;
	StackObject<Buffer> ShadowMap::s_defaultRenderSettingsBuffer;
	
	ShadowMap::ShadowMap(bool isStatic)
	    : m_renderSettingsBuffer(BufferAllocator::GetInstance().AllocateUnique(sizeof(float) * 16, GL_MAP_WRITE_BIT)),
	      m_isStatic(isStatic)
	{
		
	}
	
	void ShadowMap::SetResolution(GLsizei width, GLsizei height)
	{
		if (m_width == width && m_height == height)
			return;
		m_width = width;
		m_height = height;
		
		const double SHADOW_PASS_RESOLUTION_MUL = 0.25;
		GLsizei shadowPassTextureWidth = static_cast<GLsizei>(width * SHADOW_PASS_RESOLUTION_MUL);
		GLsizei shadowPassTextureHeight = static_cast<GLsizei>(height * SHADOW_PASS_RESOLUTION_MUL);
		
		m_shadowPassTexture.Construct(shadowPassTextureWidth, shadowPassTextureHeight, 1, GL_R8);
		m_shadowPassTexture->SetWrapMode(GL_CLAMP_TO_EDGE);
		m_shadowPassTexture->SetMagFilter(GL_LINEAR);
		m_shadowPassTexture->SetMinFilter(GL_LINEAR);
		
		m_shadowPassFramebuffer.Construct();
		glNamedFramebufferTexture(m_shadowPassFramebuffer->GetID(), GL_COLOR_ATTACHMENT0, m_shadowPassTexture->GetID(), 0);
		glNamedFramebufferDrawBuffer(m_shadowPassFramebuffer->GetID(), GL_COLOR_ATTACHMENT0);
		
		m_blurPassTexture.Construct(width, height, 1, GL_R8);
		m_blurPassTexture->SetWrapMode(GL_CLAMP_TO_EDGE);
		m_blurPassTexture->SetMagFilter(GL_NEAREST);
		m_blurPassTexture->SetMinFilter(GL_NEAREST);
		
		m_blurPassFramebuffer.Construct();
		glNamedFramebufferTexture(m_blurPassFramebuffer->GetID(), GL_COLOR_ATTACHMENT0, m_blurPassTexture->GetID(), 0);
		glNamedFramebufferDrawBuffer(m_blurPassFramebuffer->GetID(), GL_COLOR_ATTACHMENT0);
	}
	
	glm::mat3 ShadowMap::GetInverseStaticViewMatrix(LightInfo lightInfo) const
	{
		return glm::transpose(glm::mat3(
			1.0f, 0.0f, lightInfo.m_position.x,
			0.0f, 1.0f, lightInfo.m_position.y,
			0.0f, 0.0f, 1.0f
		)) * glm::transpose(glm::mat3(
			lightInfo.m_range, 0.0f, 0.0f,
			0.0f, lightInfo.m_range, 0.0f,
			0.0f, 0.0f, 1.0f
		));
	}
	
	glm::mat3 ShadowMap::GetStaticViewMatrix(LightInfo lightInfo) const
	{
		float scale = 1.0f / lightInfo.m_range;
		
		return glm::transpose(glm::mat3(
			scale, 0.0f, 0.0f,
			0.0f, scale, 0.0f,
			0.0f, 0.0f, 1.0f
		)) * glm::transpose(glm::mat3(
			1.0f, 0.0f, -lightInfo.m_position.x,
			0.0f, 1.0f, -lightInfo.m_position.y,
			0.0f, 0.0f, 1.0f
		));
	}
	
	void ShadowMap::BeginShadowPass(const ViewInfo& viewInfo, LightInfo lightInfo) const
	{
		Framebuffer::Bind(*m_shadowPassFramebuffer, 0, 0, m_shadowPassTexture->GetWidth(), m_shadowPassTexture->GetHeight());
		
		float projectionSphereRadius;
		
		if (m_isStatic)
		{
			projectionSphereRadius = std::sqrt(2.0f) * lightInfo.m_range;
		}
		else
		{
			float maxDistToViewCornerSq = 0;
			for (size_t i = 0; i < 4; i++)
			{
				float distSq = LengthSquared(viewInfo.GetWorldViewCorners()[i] - lightInfo.m_position);
				if (distSq > maxDistToViewCornerSq)
					maxDistToViewCornerSq = distSq;
			}
			projectionSphereRadius = std::sqrt(maxDistToViewCornerSq);
		}
		
		glm::mat3 viewMatrix = m_isStatic ? GetStaticViewMatrix(lightInfo) : viewInfo.GetViewMatrix();
		
		void* memory = glMapNamedBuffer(*m_renderSettingsBuffer, GL_WRITE_ONLY);
		
		memcpy(reinterpret_cast<float*>(memory) + 0, &viewMatrix[0], sizeof(float) * 3);
		memcpy(reinterpret_cast<float*>(memory) + 4, &viewMatrix[1], sizeof(float) * 3);
		memcpy(reinterpret_cast<float*>(memory) + 8, &viewMatrix[2], sizeof(float) * 3);
		
		*(reinterpret_cast<float*>(memory) + 12) = lightInfo.m_position.x;
		*(reinterpret_cast<float*>(memory) + 13) = lightInfo.m_position.y;
		*(reinterpret_cast<float*>(memory) + 14) = projectionSphereRadius;
		
		glUnmapNamedBuffer(*m_renderSettingsBuffer);
		
		
		float CLEAR_VALUE = 0.0f;
		glClearNamedFramebufferfv(m_shadowPassFramebuffer->GetID(), GL_COLOR, 0, &CLEAR_VALUE);
		
		glBindBufferBase(GL_UNIFORM_BUFFER, 3, *m_renderSettingsBuffer);
	}
	
	void ShadowMap::BeginBlurPass() const
	{
		Framebuffer::Bind(*m_blurPassFramebuffer, 0, 0, m_blurPassTexture->GetWidth(), m_blurPassTexture->GetHeight());
		
		m_shadowPassTexture->Bind(0);
	}
	
	void ShadowMap::BindDefault()
	{
		if (s_defaultTexture.IsNull())
		{
			s_defaultTexture.Construct(2, 2, 1, GL_R8);
			
			float COLOR = 0.0f;
			glClearTexImage(s_defaultTexture->GetID(), 0, GL_RED, GL_FLOAT, &COLOR);
			
			float bufferContents[16];
			std::fill(bufferContents, std::end(bufferContents), 0.0f);
			s_defaultRenderSettingsBuffer.Construct(sizeof(bufferContents), bufferContents, 0);
			
			CallOnClose([] { s_defaultTexture.Destroy(); s_defaultRenderSettingsBuffer.Destroy(); });
		}
		
		s_defaultTexture->Bind(1);
		glBindBufferBase(GL_UNIFORM_BUFFER, 2, s_defaultRenderSettingsBuffer->GetID());
	}
	
	void ShadowMap::Bind() const
	{
		if (m_blurPassTexture.IsNull())
			BindDefault();
		else
		{
			m_blurPassTexture->Bind(1);
			glBindBufferBase(GL_UNIFORM_BUFFER, 2, *m_renderSettingsBuffer);
		}
	}
}
