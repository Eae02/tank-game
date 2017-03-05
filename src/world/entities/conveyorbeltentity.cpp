#include "conveyorbeltentity.h"
#include "../../orientedrectangle.h"
#include "../../graphics/spriterenderlist.h"
#include "../../graphics/textureloadoperation.h"
#include "../../graphics/gl/shadermodule.h"
#include "../../graphics/quadmesh.h"
#include "../../asyncworklist.h"
#include "../../utils/ioutils.h"
#include "../../utils/utils.h"
#include "../../updateinfo.h"

#include <imgui.h>

namespace TankGame
{
	std::unique_ptr<Texture2D> ConveyorBeltEntity::s_diffuseTexture;
	std::unique_ptr<Texture2D> ConveyorBeltEntity::s_normalMap;
	std::unique_ptr<Texture2D> ConveyorBeltEntity::s_specularTexture;
	
	std::unique_ptr<ShaderProgram> ConveyorBeltEntity::s_shader;
	
	ConveyorBeltEntity::ConveyorBeltEntity(float speed)
	    : m_uniformBuffer(BufferAllocator::GetInstance().AllocateUnique(sizeof(float) * 15, GL_MAP_WRITE_BIT)),
	      m_speed(speed)
	{
		if (s_shader == nullptr)
		{
			auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "conveyor.vs.glsl", GL_VERTEX_SHADER);
			auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "conveyor.fs.glsl", GL_FRAGMENT_SHADER);
			
			s_shader.reset(new ShaderProgram{ &vs, &fs });
			
			CallOnClose([] { s_shader = nullptr; });
		}
	}
	
	void ConveyorBeltEntity::Update(const UpdateInfo& updateInfo)
	{
		if (m_enabled)
			m_textureOffset += updateInfo.m_dt * m_speed;
	}
	
	void ConveyorBeltEntity::Draw(SpriteRenderList& spriteRenderList) const
	{
		void* bufferMemory = glMapNamedBufferRange(*m_uniformBuffer, 0, sizeof(float) * 15,
		                                           GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		
		memcpy(reinterpret_cast<float*>(bufferMemory) + 0, &GetTransform().GetMatrix()[0], sizeof(float) * 3);
		memcpy(reinterpret_cast<float*>(bufferMemory) + 4, &GetTransform().GetMatrix()[1], sizeof(float) * 3);
		memcpy(reinterpret_cast<float*>(bufferMemory) + 8, &GetTransform().GetMatrix()[2], sizeof(float) * 3);
		reinterpret_cast<float*>(bufferMemory)[12] = GetTransform().GetScale().x;
		reinterpret_cast<float*>(bufferMemory)[13] = GetTransform().GetScale().y;
		reinterpret_cast<float*>(bufferMemory)[14] = m_textureOffset;
		
		glUnmapNamedBuffer(*m_uniformBuffer);
		
		s_shader->Use();
		
		s_diffuseTexture->Bind(0);
		s_normalMap->Bind(1);
		s_specularTexture->Bind(2);
		
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, *m_uniformBuffer);
		
		QuadMesh::GetInstance().GetVAO().Bind();
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	const char* ConveyorBeltEntity::GetSerializeClassName() const
	{ return "ConveyorBelt"; }
	
	nlohmann::json ConveyorBeltEntity::Serialize() const
	{
		nlohmann::json json = Entity::Serialize();
		
		json["enabled"] = m_enabled;
		json["speed"] = m_speed;
		
		return json;
	}
	
	void ConveyorBeltEntity::RenderProperties()
	{
		RenderBaseProperties();
		
		ImGui::Checkbox("Enabled", &m_enabled);
		
		ImGui::InputFloat("Speed", &m_speed);
	}
	
	const char*ConveyorBeltEntity::GetObjectName() const
	{ return "Conveyor Belt"; }
	
	std::unique_ptr<Entity> ConveyorBeltEntity::Clone() const
	{
		std::unique_ptr<ConveyorBeltEntity> clone = std::make_unique<ConveyorBeltEntity>(m_speed);
		clone->GetTransform() = GetTransform();
		clone->m_enabled = m_enabled;
		return clone;
	}
	
	glm::vec2 ConveyorBeltEntity::GetPushVector(glm::vec2 position) const
	{
		OrientedRectangle rectangle = OrientedRectangle::FromTransformedNDC(GetTransform());
		
		if (rectangle.Contains(position))
			return GetTransform().GetForward() * m_speed;
		return glm::vec2(0.0f);
	}
	
	void ConveyorBeltEntity::LoadResources(ASyncWorkList& workList)
	{
		workList.SubmitWork(std::make_unique<TextureLoadOperation>(GetResDirectory() / "conveyor-diffuse.png",
		                                                           [] (Texture2D&& result)
		{
			s_diffuseTexture = std::make_unique<Texture2D>(std::move(result));
			s_diffuseTexture->SetWrapMode(GL_REPEAT);
		}));
		
		workList.SubmitWork(std::make_unique<TextureLoadOperation>(GetResDirectory() / "conveyor-normals.png",
		                                                           [] (Texture2D&& result)
		{
			s_normalMap = std::make_unique<Texture2D>(std::move(result));
			s_normalMap->SetWrapMode(GL_REPEAT);
		}));
		
		workList.SubmitWork(std::make_unique<TextureLoadOperation>(GetResDirectory() / "conveyor-specular.png",
		                                                           [] (Texture2D&& result)
		{
			s_specularTexture = std::make_unique<Texture2D>(std::move(result));
			s_specularTexture->SetWrapMode(GL_REPEAT);
		}));
		
		CallOnClose([] { s_diffuseTexture = nullptr; s_normalMap = nullptr; s_specularTexture = nullptr; });
	}
}
