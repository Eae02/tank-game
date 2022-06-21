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
	Lua::RegistryReference ConveyorBeltEntity::s_metaTableRef;
	
	std::unique_ptr<Texture2D> ConveyorBeltEntity::s_diffuseTexture;
	std::unique_ptr<Texture2D> ConveyorBeltEntity::s_normalMap;
	std::unique_ptr<Texture2D> ConveyorBeltEntity::s_specularTexture;
	
	std::unique_ptr<ShaderProgram> ConveyorBeltEntity::s_shader;
	
	int ConveyorBeltEntity::s_transformUniformLoc;
	int ConveyorBeltEntity::s_sizeUniformLoc;
	int ConveyorBeltEntity::s_textureOffsetUniformLoc;
	
	ConveyorBeltEntity::ConveyorBeltEntity(float speed)
	    : m_speed(speed)
	{
		if (s_shader == nullptr)
		{
			auto vs = ShaderModule::FromFile(resDirectoryPath / "shaders" / "conveyor.vs.glsl", GL_VERTEX_SHADER);
			auto fs = ShaderModule::FromFile(resDirectoryPath / "shaders" / "conveyor.fs.glsl", GL_FRAGMENT_SHADER);
			
			s_shader.reset(new ShaderProgram{ &vs, &fs });
			s_shader->SetTextureBinding("diffuseSampler", 0);
			s_shader->SetTextureBinding("normalMapSampler", 1);
			s_shader->SetTextureBinding("specularMapSampler", 2);
			
			s_transformUniformLoc     = s_shader->GetUniformLocation("transform");
			s_sizeUniformLoc          = s_shader->GetUniformLocation("size");
			s_textureOffsetUniformLoc = s_shader->GetUniformLocation("textureOffset");
			
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
		s_shader->Use();
		
		glUniformMatrix3fv(s_transformUniformLoc, 1, false, &GetTransform().GetMatrix()[0][0]);
		glUniform2f(s_sizeUniformLoc, GetTransform().GetScale().x, GetTransform().GetScale().y);
		glUniform1f(s_textureOffsetUniformLoc, m_textureOffset);
		
		s_diffuseTexture->Bind(0);
		s_normalMap->Bind(1);
		s_specularTexture->Bind(2);
		
		QuadMesh::GetInstance().BindVAO();
		
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
	
	void ConveyorBeltEntity::PushLuaMetaTable(lua_State* state) const
	{
		if (!s_metaTableRef)
		{
			NewLuaMetaTable(state);
			
			Entity::PushLuaMetaTable(state);
			lua_setmetatable(state, -2);
			
			// ** setSpeed **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				dynamic_cast<ConveyorBeltEntity*>(LuaGetInstance(state))->SetSpeed(luaL_checknumber(state, 2));
				return 0;
			});
			lua_setfield(state, -2, "setSpeed");
			
			// ** getSpeed **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				lua_pushnumber(state, dynamic_cast<const ConveyorBeltEntity*>(LuaGetInstance(state))->GetSpeed());
				return 1;
			});
			lua_setfield(state, -2, "getSpeed");
			
			s_metaTableRef = Lua::RegistryReference::PopAndCreate(state);
			CallOnClose([] { s_metaTableRef = { }; });
		}
		
		s_metaTableRef.Load(state);
	}
	
	void ConveyorBeltEntity::LoadResources(ASyncWorkList& workList)
	{
		workList.Add(TextureLoadOperation::Start(resDirectoryPath / "conveyor-diffuse.png"), [] (TextureLoadOperation op)
		{
			s_diffuseTexture = std::make_unique<Texture2D>(op.CreateTexture());
			s_diffuseTexture->SetWrapMode(GL_REPEAT);
		});
		
		workList.Add(TextureLoadOperation::Start(resDirectoryPath / "conveyor-normals.png"), [] (TextureLoadOperation op)
		{
			s_normalMap = std::make_unique<Texture2D>(op.CreateTexture());
			s_normalMap->SetWrapMode(GL_REPEAT);
		});
		
		workList.Add(TextureLoadOperation::Start(resDirectoryPath / "conveyor-specular.png"), [] (TextureLoadOperation op)
		{
			s_specularTexture = std::make_unique<Texture2D>(op.CreateTexture());
			s_specularTexture->SetWrapMode(GL_REPEAT);
		});
		
		CallOnClose([] { s_diffuseTexture = nullptr; s_normalMap = nullptr; s_specularTexture = nullptr; });
	}
}
