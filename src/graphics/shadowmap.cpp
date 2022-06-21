#include <cstring>
#include "shadowmap.h"
#include "../utils/utils.h"
#include "../utils/mathutils.h"

namespace TankGame
{
	std::unique_ptr<Texture2D> ShadowMap::s_defaultTexture;
	std::unique_ptr<Buffer> ShadowMap::s_defaultRenderSettingsBuffer;
	
	static constexpr size_t SHADOW_RENDER_SETTINGS_SIZE = sizeof(float) * 16;
	
	void ShadowMap::SetResolution(GLsizei width, GLsizei height)
	{
		if (m_width == width && m_height == height)
			return;
		m_width = width;
		m_height = height;
		
		const double SHADOW_PASS_RESOLUTION_MUL = 0.25;
		GLsizei shadowPassTextureWidth = static_cast<GLsizei>(width * SHADOW_PASS_RESOLUTION_MUL);
		GLsizei shadowPassTextureHeight = static_cast<GLsizei>(height * SHADOW_PASS_RESOLUTION_MUL);
		
		m_shadowPassTexture = std::make_unique<Texture2D>(shadowPassTextureWidth, shadowPassTextureHeight, 1, GL_DEPTH_COMPONENT16);
		m_shadowPassTexture->SetWrapMode(GL_CLAMP_TO_EDGE);
		m_shadowPassTexture->SetMagFilter(GL_LINEAR);
		m_shadowPassTexture->SetMinFilter(GL_LINEAR);
		
		m_shadowPassFramebuffer = std::make_unique<Framebuffer>();
		glNamedFramebufferTexture(m_shadowPassFramebuffer->GetID(), GL_DEPTH_ATTACHMENT, m_shadowPassTexture->GetID(), 0);
		
		m_blurPassTexture = std::make_unique<Texture2D>(width, height, 1, GL_R8);
		m_blurPassTexture->SetWrapMode(GL_CLAMP_TO_EDGE);
		m_blurPassTexture->SetMagFilter(GL_NEAREST);
		m_blurPassTexture->SetMinFilter(GL_NEAREST);
		
		m_blurPassFramebuffer = std::make_unique<Framebuffer>();
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
	
	void ShadowMap::BeginShadowPass(const ViewInfo& viewInfo, LightInfo lightInfo)
	{
		Framebuffer::Bind(*m_shadowPassFramebuffer, 0, 0, m_shadowPassTexture->GetWidth(), m_shadowPassTexture->GetHeight());
		
		float projectionSphereRadius = std::sqrt(2.0f) * lightInfo.m_range;
		
		glm::mat3 viewMatrix = m_isStatic ? GetStaticViewMatrix(lightInfo) : viewInfo.GetViewMatrix();
		
		m_currentRenderSettingsBuffer++;
		if (m_currentRenderSettingsBuffer == MAX_QUEUED_FRAMES)
			m_currentRenderSettingsBuffer = 0;
		if (m_renderSettingsBuffers[m_currentRenderSettingsBuffer].IsNull())
		{
			m_renderSettingsBuffers[m_currentRenderSettingsBuffer] =
				BufferAllocator::GetInstance().AllocateUnique(SHADOW_RENDER_SETTINGS_SIZE, BufferUsage::MapWritePersistent);
		}
		
		float* memory = reinterpret_cast<float*>(m_renderSettingsBuffers[m_currentRenderSettingsBuffer]->MappedMemory());
		memcpy(memory + 0, &viewMatrix[0], sizeof(float) * 3);
		memcpy(memory + 4, &viewMatrix[1], sizeof(float) * 3);
		memcpy(memory + 8, &viewMatrix[2], sizeof(float) * 3);
		memory[12] = lightInfo.m_position.x;
		memory[13] = lightInfo.m_position.y;
		memory[14] = projectionSphereRadius;
		
		m_renderSettingsBuffers[m_currentRenderSettingsBuffer]->FlushMappedMemory(0, SHADOW_RENDER_SETTINGS_SIZE);
		
		float CLEAR_VALUE = 0.0f;
		glClearNamedFramebufferfv(m_shadowPassFramebuffer->GetID(), GL_DEPTH, 0, &CLEAR_VALUE);
		
		glBindBufferBase(GL_UNIFORM_BUFFER, SHADOW_RENDER_SETTINGS_BUFFER_BINDING,
		                 m_renderSettingsBuffers[m_currentRenderSettingsBuffer]->GetID());
	}
	
	void ShadowMap::BeginBlurPass() const
	{
		Framebuffer::Bind(*m_blurPassFramebuffer, 0, 0, m_blurPassTexture->GetWidth(), m_blurPassTexture->GetHeight());
		
		m_shadowPassTexture->Bind(0);
	}
	
	void ShadowMap::BindDefault(int textureBindUnit)
	{
		if (s_defaultTexture == nullptr)
		{
			s_defaultTexture = std::make_unique<Texture2D>(1, 1, 1, GL_R8);
			
			float COLOR = 0.0f;
			glTextureSubImage2D(s_defaultTexture->GetID(), 0, 0, 0, 1, 1, GL_RGBA, GL_FLOAT, &COLOR);
			
			float bufferContents[16];
			std::fill(bufferContents, std::end(bufferContents), 0.0f);
			s_defaultRenderSettingsBuffer = std::make_unique<Buffer>(sizeof(bufferContents), bufferContents, BufferUsage::StaticData);
			
			CallOnClose([] { s_defaultTexture = nullptr; s_defaultRenderSettingsBuffer = nullptr; });
		}
		
		s_defaultTexture->Bind(textureBindUnit);
		glBindBufferBase(GL_UNIFORM_BUFFER, SHADOW_RENDER_SETTINGS_BUFFER_BINDING, s_defaultRenderSettingsBuffer->GetID());
	}
	
	void ShadowMap::Bind(int textureBindUnit) const
	{
		if (m_blurPassTexture == nullptr)
		{
			BindDefault(textureBindUnit);
		}
		else
		{
			m_blurPassTexture->Bind(1);
			glBindBufferBase(GL_UNIFORM_BUFFER, SHADOW_RENDER_SETTINGS_BUFFER_BINDING,
			                 m_renderSettingsBuffers[m_currentRenderSettingsBuffer]->GetID());
		}
	}
}
