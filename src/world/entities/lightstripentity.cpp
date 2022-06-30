#include "lightstripentity.h"
#include "../path/path.h"
#include "../gameworld.h"
#include "../lights/raylightentity.h"
#include "../../graphics/gl/shadermodule.h"
#include "../../utils/utils.h"
#include "../../utils/ioutils.h"
#include "../../utils/mathutils.h"
#include "../../lua/luainc.h"

#include <nlohmann/json.hpp>
#include <imgui.h>
#include <glm/gtc/color_space.hpp>

namespace TankGame
{
	Lua::RegistryReference LightStripEntity::s_metaTableRef;
	
	std::unique_ptr<ShaderProgram> LightStripEntity::s_shader;
	int LightStripEntity::s_colorUniformLoc;
	
	void LightStripEntity::BindShader()
	{
		if (s_shader== nullptr)
		{
			s_shader = std::make_unique<ShaderProgram>(
				ShaderModule::FromResFile("lightstrip.vs.glsl"), ShaderModule::FromResFile("lightstrip.fs.glsl"));
			s_colorUniformLoc = s_shader->GetUniformLocation("color");
			
			CallOnClose([] { s_shader = nullptr; });
		}
		
		s_shader->Use();
	}
	
	void LightStripEntity::Draw(class SpriteRenderList& spriteRenderList) const
	{
		if (m_numIndices != 0)
		{
			BindShader();
			
			glm::vec3 colorMulStrength = m_color * m_glowStrength;
			glUniform3fv(s_colorUniformLoc, 1, &colorMulStrength.x);
			
			m_vertexInputState.Bind();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer->GetID());
			glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_SHORT, nullptr);
		}
	}
	
	void LightStripEntity::SetPath(Path path)
	{
		m_path = std::move(path);
		PathChanged();
	}
	
	std::unique_ptr<Entity> LightStripEntity::Clone() const
	{
		auto clone = std::make_unique<LightStripEntity>(m_color, m_glowStrength, m_radius);
		
		clone->GetTransform() = GetTransform();
		clone->SetPath(m_path);
		
		return clone;
	}
	
	void LightStripEntity::EditorMoved()
	{
		glm::vec2 deltaMove = GetTransform().GetPosition() - m_centerPath;
		m_centerPath += deltaMove;
		
		for (size_t i = 0; i < m_path.GetNodeCount(); i++)
			m_path[i] += deltaMove;
		PathChanged();
	}
	
	void LightStripEntity::EditorSpawned()
	{
		for (size_t i = 0; i < m_path.GetNodeCount(); i++)
			m_path[i] += GetTransform().GetPosition();
		PathChanged();
	}
	
	void LightStripEntity::SpawnLights(const Path& path)
	{
		assert(path.GetNodeCount() != 0);
		
		for (RayLightEntity* light : m_lights)
			light->Despawn();
		m_lights.resize(path.GetNodeCount() - 1);
		
		for (size_t i = 0; i < path.GetNodeCount() - 1; i++)
		{
			glm::vec2 toNext = path[i + 1] - path[i];
			
			auto entity = std::make_unique<RayLightEntity>(m_color, m_glowStrength * 0.25f,
			                                               Attenuation(), glm::length(toNext), 0.1f);
			
			entity->GetTransform().SetPosition(path[i]);
			entity->GetTransform().SetRotation(glm::half_pi<float>() + std::atan2(toNext.y, toNext.x));
			
			entity->SetEditorVisible(false);
			
			m_lights[i] = entity.get();
			
			GetGameWorld()->Spawn(std::move(entity));
		}
	}
	
	void LightStripEntity::PathChanged()
	{
		glm::vec2 maxPosition = m_path[0];
		glm::vec2 minPosition = m_path[0];
		for (size_t i = 1; i < m_path.GetNodeCount(); i++)
		{
			maxPosition = glm::max(maxPosition, m_path[i]);
			minPosition = glm::min(minPosition, m_path[i]);
		}
		
		m_centerPath = (maxPosition + minPosition) * 0.5f;
		GetTransform().SetPosition(m_centerPath);
		
		std::vector<glm::vec2> vertices;
		std::vector<uint16_t> indices;
		
		vertices.reserve(m_path.GetNodeCount() * 2);
		indices.reserve((m_path.GetNodeCount() - 1) * 6);
		
		glm::vec2 preToVertex;
		
		for (size_t i = 0; i < m_path.GetNodeCount(); i++)
		{
			glm::vec2 toPrev, toNext;
			
			bool isFirstElement = i == 0;
			bool isLastElement = i == m_path.GetNodeCount() - 1;
			
			if (!isFirstElement)
				toPrev = glm::normalize(m_path[i - 1] - m_path[i]);
			if (!isLastElement)
				toNext = glm::normalize(m_path[i + 1] - m_path[i]);
			
			if (isFirstElement)
				toPrev = -toNext;
			if (isLastElement)
				toNext = -toPrev;
			
			glm::vec2 n1(-toNext.y, toNext.x);
			glm::vec2 n2(toPrev.y, -toPrev.x);
			
			float cosT = glm::dot(n1, n2);
			float tanHalfT = std::abs(cosT - 1) < 1E-6 ? 0 : (1.0f - cosT) / std::sqrt(1.0f - cosT * cosT);
			
			glm::vec2 toVertex = (n1 - toNext * tanHalfT) * m_radius;
			
			if (i != 0)
			{
				if (glm::dot(toVertex, preToVertex) < 0)
					toVertex *= -1;
				preToVertex = toVertex;
			}
			
			vertices.push_back(m_path[i] - toVertex);
			vertices.push_back(m_path[i] + toVertex);
			
			if (i != 0)
			{
				indices.push_back(vertices.size() - 4);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 3);
				indices.push_back(vertices.size() - 2);
				indices.push_back(vertices.size() - 1);
			}
		}
		
		glm::vec2 max = vertices[0];
		glm::vec2 min = vertices[0];
		for (size_t i = 1; i < vertices.size(); i++)
		{
			max = glm::max(max, vertices[i]);
			min = glm::min(min, vertices[i]);
		}
		
		glm::vec2 center = (min + max) / 2.0f;
		float maxDistanceToCenterSq = 0;
		for (size_t i = 0; i < vertices.size(); i++)
		{
			maxDistanceToCenterSq = glm::max(glm::distance2(vertices[i], center), maxDistanceToCenterSq);
		}
		
		m_boundingCircle = { center, std::sqrt(maxDistanceToCenterSq) };
		
		m_numIndices = indices.size();
		if (!vertices.empty())
		{
			m_vertexBuffer = std::make_unique<Buffer>(vertices.size() * sizeof(glm::vec2), vertices.data(), BufferUsage::StaticVertex);
			m_indexBuffer = std::make_unique<Buffer>(indices.size() * sizeof(uint16_t), indices.data(), BufferUsage::StaticIndex);
			
			m_vertexInputState.UpdateAttribute(0, m_vertexBuffer->GetID(), VertexAttribFormat::Float32_2, 0, sizeof(float) * 2);
		}
		
		if (GetGameWorld() != nullptr)
			SpawnLights(m_path);
	}
	
	Circle LightStripEntity::GetBoundingCircle() const
	{
		return m_boundingCircle;
	}
	
	void LightStripEntity::OnSpawned(GameWorld& gameWorld)
	{
		Entity::OnSpawned(gameWorld);
		if (m_path.GetNodeCount() > 0)
			SpawnLights(m_path);
	}
	
	void LightStripEntity::OnDespawning()
	{
		for (RayLightEntity* lightEntity : m_lights)
			lightEntity->Despawn();
	}
	
	void LightStripEntity::SetColor(glm::vec3 color)
	{
		m_color = color;
		for (RayLightEntity* light : m_lights)
			light->SetColor(color);
	}
	
	void LightStripEntity::SetGlowStrength(float glowStrength)
	{
		m_glowStrength = glowStrength;
		for (RayLightEntity* light : m_lights)
			light->SetIntensity(m_glowStrength * 0.25f);
	}
	
	void LightStripEntity::SetRadius(float radius)
	{
		m_radius = radius;
		PathChanged();
	}
	
	void LightStripEntity::RenderProperties()
	{
		RenderBaseProperties(Transform::Properties::None);
		
		glm::vec3 colorSrgb = glm::convertLinearToSRGB(m_color);
		if (ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&colorSrgb)))
			SetColor(glm::convertSRGBToLinear(colorSrgb));
		
		float radius = m_radius;
		if (ImGui::InputFloat("Radius", &radius))
			SetRadius(glm::max(0.0f, radius));
		
		float glowStrength = m_glowStrength;
		if (ImGui::InputFloat("Glow Strength", &glowStrength))
			SetGlowStrength(glm::max(0.0f, glowStrength));
	}
	
	const char* LightStripEntity::GetObjectName() const
	{
		return "Light Strip";
	}
	
	const char* LightStripEntity::GetSerializeClassName() const
	{
		return "LightStrip";
	}
	
	nlohmann::json LightStripEntity::Serialize() const
	{
		nlohmann::json json;
		
		uint32_t hexColor = RgbColorToSRGBHex(m_color);
		char hexColorStr[7];
		sprintf(hexColorStr, "%06x", hexColor);
		
		json["color"] = hexColorStr;
		
		json["glow_strength"] = m_glowStrength;
		json["radius"] = m_radius;
		
		nlohmann::json::array_t nodesEl;
		for (size_t i = 0; i < m_path.GetNodeCount(); i++)
			nodesEl.push_back(nlohmann::json::array({ m_path[i].x, m_path[i].y }));
		
		json["nodes"] = nodesEl;
		
		return json;
	}
	
	Path& LightStripEntity::GetEditPath()
	{
		return m_path;
	}
	
	void LightStripEntity::PathEditEnd()
	{
		PathChanged();
	}
	
	bool LightStripEntity::IsClosedPath() const
	{
		return false;
	}
	
	const char* LightStripEntity::GetEditPathName() const
	{
		return "Light Path";
	}
	
	const glm::vec3 activatedColor = glm::convertSRGBToLinear(glm::vec3(0.09f, 0.89f, 0.0f));
	
	void LightStripEntity::PushLuaMetaTable(lua_State* state) const
	{
		if (!s_metaTableRef)
		{
			NewLuaMetaTable(state);
			
			Entity::PushLuaMetaTable(state);
			lua_setmetatable(state, -2);
			
			// ** setColor **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				glm::vec3 color(luaL_checknumber(state, 2), luaL_checknumber(state, 3), luaL_checknumber(state, 4));
				
				dynamic_cast<LightStripEntity*>(LuaGetInstance(state))->SetColor(glm::convertSRGBToLinear(color));
				return 0;
			});
			lua_setfield(state, -2, "setColor");
			
			// ** activate **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				dynamic_cast<LightStripEntity*>(LuaGetInstance(state))->SetColor(activatedColor);
				return 0;
			});
			lua_setfield(state, -2, "activate");
			
			s_metaTableRef = Lua::RegistryReference::PopAndCreate(state);
			CallOnClose([] { s_metaTableRef = { }; });
		}
		
		s_metaTableRef.Load(state);
	}
}
