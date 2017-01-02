#include "lightstripentity.h"
#include "../path/path.h"
#include "../gameworld.h"
#include "../lights/raylightentity.h"
#include "../../graphics/gl/shadermodule.h"
#include "../../utils/utils.h"
#include "../../utils/ioutils.h"
#include "../../utils/mathutils.h"

#include <imgui.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/color_space.hpp>

namespace TankGame
{
	std::unique_ptr<ShaderProgram> LightStripEntity::s_shader;
	
	void LightStripEntity::BindShader()
	{
		if (s_shader== nullptr)
		{
			auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "lightstrip.vs.glsl", GL_VERTEX_SHADER);
			auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "lightstrip.fs.glsl", GL_FRAGMENT_SHADER);
			
			s_shader.reset(new ShaderProgram{ &vs, &fs });
			
			CallOnClose([] { s_shader = nullptr; });
		}
		
		s_shader->Use();
	}
	
	LightStripEntity::LightStripEntity(glm::vec3 color, float glowStrength, float radius)
	    : LightStripEntity(color, glowStrength, radius, "", color) { }
	
	LightStripEntity::LightStripEntity(glm::vec3 color, float glowStrength, float radius,
	                                   std::string activateEventName, glm::vec3 activatedColor)
	    : m_uniformBuffer(BufferAllocator::GetInstance().AllocateUnique(sizeof(float) * 4, GL_MAP_WRITE_BIT)),
	      m_radius(radius), m_activateEvent(std::move(activateEventName)),
	      m_activatedColor(activatedColor), m_color(color), m_glowStrength(glowStrength)
	{
		glEnableVertexArrayAttrib(m_vertexArray.GetID(), 0);
		glVertexArrayAttribFormat(m_vertexArray.GetID(), 0, 2, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(m_vertexArray.GetID(), 0, 0);
	}
	
	void LightStripEntity::Draw(class SpriteRenderList& spriteRenderList) const
	{
		if (m_uniformBufferOutOfDate)
		{
			float* memory = reinterpret_cast<float*>(glMapNamedBufferRange(*m_uniformBuffer, 0, sizeof(float) * 3,
					GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
			
			memory[0] = m_color.r * m_glowStrength;
			memory[1] = m_color.g * m_glowStrength;
			memory[2] = m_color.b * m_glowStrength;
			
			glUnmapNamedBuffer(*m_uniformBuffer);
			
			m_uniformBufferOutOfDate = false;
		}
		
		if (m_numIndices != 0)
		{
			BindShader();
			
			glBindBufferBase(GL_UNIFORM_BUFFER, 1, *m_uniformBuffer);
			
			m_vertexArray.Bind();
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
		auto clone = std::make_unique<LightStripEntity>(m_color, m_glowStrength, m_radius, m_activateEvent, m_activatedColor);
		
		clone->GetTransform() = GetTransform();
		clone->SetPath(m_path);
		
		return clone;
	}
	
	void LightStripEntity::HandleEvent(const std::string& event, Entity* sender)
	{
		if (event == "EditorMoved")
		{
			glm::vec2 deltaMove = GetTransform().GetPosition() - m_centerPath;
			m_centerPath += deltaMove;
			
			for (size_t i = 0; i < m_path.GetNodeCount(); i++)
				m_path[i] += deltaMove;
			PathChanged();
		}
		else if (event == "EditorSpawned")
		{
			for (size_t i = 0; i < m_path.GetNodeCount(); i++)
				m_path[i] += GetTransform().GetPosition();
			PathChanged();
		}
		else if (event == m_activateEvent)
		{
			SetColor(m_activatedColor);
		}
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
			maxDistanceToCenterSq = glm::max(LengthSquared(vertices[i] - center), maxDistanceToCenterSq);
		}
		
		m_boundingCircle = { center, std::sqrt(maxDistanceToCenterSq) };
		
		m_numIndices = indices.size();
		if (!vertices.empty())
		{
			m_vertexBuffer = std::make_unique<Buffer>(vertices.size() * sizeof(glm::vec2), vertices.data(), 0);
			m_indexBuffer = std::make_unique<Buffer>(indices.size() * sizeof(uint16_t), indices.data(), 0);
			
			glVertexArrayVertexBuffer(m_vertexArray.GetID(), 0, m_vertexBuffer->GetID(), 0, sizeof(float) * 2);
			glVertexArrayElementBuffer(m_vertexArray.GetID(), m_indexBuffer->GetID());
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
		
		if (gameWorld.GetWorldType() == GameWorld::Types::Game && !m_activateEvent.empty())
			gameWorld.ListenForEvent(m_activateEvent, *this);
	}
	
	void LightStripEntity::OnDespawning()
	{
		for (RayLightEntity* lightEntity : m_lights)
			lightEntity->Despawn();
	}
	
	void LightStripEntity::SetColor(glm::vec3 color)
	{
		m_color = color;
		m_uniformBufferOutOfDate = true;
		
		for (RayLightEntity* light : m_lights)
			light->SetColor(color);
	}
	
	void LightStripEntity::SetGlowStrength(float glowStrength)
	{
		m_glowStrength = glowStrength;
		m_uniformBufferOutOfDate = true;
		
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
		glm::vec3 colorSrgb = glm::convertLinearToSRGB(m_color);
		if (ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&colorSrgb)))
			SetColor(glm::convertSRGBToLinear(colorSrgb));
		
		float radius = m_radius;
		if (ImGui::InputFloat("Radius", &radius))
			SetRadius(glm::max(0.0f, radius));
		
		float glowStrength = m_glowStrength;
		if (ImGui::InputFloat("Glow Strength", &glowStrength))
			SetGlowStrength(glm::max(0.0f, glowStrength));
		
		std::array<char, 256> activateEventBuffer;
		activateEventBuffer.back() = '\0';
		strncpy(activateEventBuffer.data(), m_activateEvent.c_str(), activateEventBuffer.size());
		
		if (ImGui::InputText("Activate Event", activateEventBuffer.data(), activateEventBuffer.size()))
			m_activateEvent = activateEventBuffer.data();
		
		glm::vec3 activatedColorSrgb = glm::convertLinearToSRGB(m_activatedColor);
		if (ImGui::ColorEdit3("Activated Color", reinterpret_cast<float*>(&activatedColorSrgb)))
			SetActivatedColor(glm::convertSRGBToLinear(activatedColorSrgb));
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
		
		if (!m_activateEvent.empty())
		{
			json["activate_event"] = m_activateEvent;
			
			uint32_t hexColor = RgbColorToSRGBHex(m_activatedColor);
			char hexColorStr[7];
			sprintf(hexColorStr, "%06x", hexColor);
			json["activated_color"] = hexColorStr;
		}
		
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
}
