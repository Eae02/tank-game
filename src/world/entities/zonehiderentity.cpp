#include "zonehiderentity.h"
#include "../gameworld.h"
#include "../../utils/utils.h"
#include "../../utils/ioutils.h"
#include "../../graphics/quadmesh.h"
#include "../../graphics/gl/shadermodule.h"
#include "../../graphics/ui/uirenderer.h"
#include "../../orientedrectangle.h"

#include <imgui.h>

namespace TankGame
{
	StackObject<ShaderProgram> ZoneHiderEntity::s_shader;
	int ZoneHiderEntity::s_transformUniformLocation;
	
	ZoneHiderEntity::ZoneHiderEntity(std::string name)
	    : m_name(std::move(name))
	{
		if (s_shader.IsNull())
		{
			auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "zonehider.vs.glsl", GL_VERTEX_SHADER);
			auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "zonehider.fs.glsl", GL_FRAGMENT_SHADER);
			
			s_shader.Construct<std::initializer_list<const ShaderModule*>>({ &vs, &fs });
			
			CallOnClose([] { s_shader.Destroy(); });
			
			s_transformUniformLocation = s_shader->GetUniformLocation("transform");
		}
	}
	
	void ZoneHiderEntity::Draw(class SpriteRenderList& spriteRenderList) const
	{
		if (!m_hidden)
			return;
		
		s_shader->Use();
		
		glProgramUniformMatrix3fv(s_shader->GetID(), s_transformUniformLocation, 1, GL_FALSE,
		                          reinterpret_cast<const GLfloat*>(&GetTransform().GetMatrix()));
		
		QuadMesh::GetInstance().GetVAO().Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	void ZoneHiderEntity::DrawEditorUI(UIRenderer& uiRenderer, const glm::mat3& uiViewMatrix) const
	{
		glm::mat3 transform = uiViewMatrix * GetTransform().GetMatrix();
		
		uiRenderer.DrawQuad(transform, glm::vec4(0.5f, 0.5f, 0.5f, 0.2f));
		uiRenderer.DrawQuadBorder(transform, glm::vec4(0.5f, 0.5f, 0.5f, 1));
	}
	
	void ZoneHiderEntity::OnSpawned(GameWorld& gameWorld)
	{
		if (gameWorld.GetWorldType() == GameWorld::Types::Game && !m_name.empty())
		{
			m_showEvent = m_name + "_show";
			m_hideEvent = m_name + "_hide";
			
			gameWorld.ListenForEvent(m_showEvent, *this);
			gameWorld.ListenForEvent(m_hideEvent, *this);
		}
		
		Entity::OnSpawned(gameWorld);
	}
	
	void ZoneHiderEntity::HandleEvent(const std::string& event, Entity* sender)
	{
		if (event == m_showEvent)
			m_hidden = false;
		else if (event == m_hideEvent)
			m_hidden = true;
	}
	
	std::unique_ptr<Entity> ZoneHiderEntity::Clone() const
	{
		std::unique_ptr<ZoneHiderEntity> entity = std::make_unique<ZoneHiderEntity>(m_name);
		
		entity->GetTransform() = GetTransform();
		
		return entity;
	}
	
	const char* ZoneHiderEntity::GetSerializeClassName() const
	{
		return "ZoneHider";
	}
	
	nlohmann::json ZoneHiderEntity::Serialize() const
	{
		nlohmann::json json;
		
		json["transform"] = GetTransform().Serialize(Transform::Properties::All);
		json["zone_name"] = m_name;
		
		return json;
	}
	
	void ZoneHiderEntity::RenderProperties()
	{
		RenderTransformProperty();
		
		std::array<char, 256> nameBuffer;
		nameBuffer.back() = '\0';
		strncpy(nameBuffer.data(), m_name.c_str(), nameBuffer.size() - 1);
		
		if (ImGui::InputText("Name", nameBuffer.data(), nameBuffer.size()))
			m_name = nameBuffer.data();
	}
	
	const char* ZoneHiderEntity::GetObjectName() const
	{
		return "Zone Hider";
	}
}
