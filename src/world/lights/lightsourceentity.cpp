#include "lightsourceentity.h"
#include "../../utils/mathutils.h"
#include "../../utils/utils.h"

#include <glm/gtc/color_space.hpp>
#include <imgui.h>
#include <cstdio>
#include <random>

namespace TankGame
{
	LightSourceEntity::LightSourceEntity(glm::vec3 color, float intensity, Attenuation attenuation, float height, size_t ubSize)
	    : m_ubSize(ubSize), m_flickerOffset(GenerateFlickerOffset()),
	      m_uniformBuffer(BufferAllocator::GetInstance().AllocateUnique(ubSize, GL_MAP_WRITE_BIT)),
	      m_color(color), m_intensity(intensity), m_attenuation(attenuation), m_height(height),
	      m_range(GetRange(color, intensity, attenuation))
	{
		
	}
	
	Circle LightSourceEntity::GetBoundingCircle() const
	{
		return Circle(GetTransform().GetPosition(), m_range);
	}
	
	void LightSourceEntity::Bind() const
	{
		if (m_uniformBufferOutOfDate)
		{
			void* bufferMemory = glMapNamedBuffer(*m_uniformBuffer, GL_WRITE_ONLY);
			
			UpdateUniformBuffer(bufferMemory);
			
			glUnmapNamedBuffer(*m_uniformBuffer);
			
			m_uniformBufferOutOfDate = false;
		}
		
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, *m_uniformBuffer);
		
		glm::vec2 pos2D = GetTransform().GetPosition();
		glUniform3f(GetPositionUniformLocation(), pos2D.x, m_height, pos2D.y);
		
		glm::mat3 worldTransform = MapNDCToRectangle(GetBoundingRectangle());
		glUniformMatrix3fv(GetWorldTransformUniformLocation(), 1, GL_FALSE, reinterpret_cast<GLfloat*>(&worldTransform));
		
		if (m_shadowMap != nullptr)
			m_shadowMap->Bind();
		else
			ShadowMap::BindDefault();
	}
	
	void LightSourceEntity::UpdateUniformBuffer(void* memory) const
	{
		float* floatMem = reinterpret_cast<float*>(memory);
		
		floatMem[0] = m_color.r;
		floatMem[1] = m_color.g;
		floatMem[2] = m_color.b;
		floatMem[3] = m_intensity;
		floatMem[4] = m_attenuation.GetLinear();
		floatMem[5] = m_attenuation.GetExponent();
		floatMem[6] = m_flickers ? 0.12f : 0.0f;
		floatMem[7] = m_flickerOffset;
	}
	
	LightInfo LightSourceEntity::GetLightInfo() const
	{
		return LightInfo(GetTransform().GetPosition(), m_range);
	}
	
	void LightSourceEntity::SetColor(const glm::vec3& color)
	{
		m_color = color;
		InvalidateUniformBuffer();
		m_range = GetRange(m_color, m_intensity, m_attenuation);
	}
	
	void LightSourceEntity::SetIntensity(float intensity)
	{
		m_intensity = intensity;
		InvalidateUniformBuffer();
		m_range = GetRange(m_color, m_intensity, m_attenuation);
	}
	
	void LightSourceEntity::SetAttenuation(const Attenuation& attenuation)
	{
		m_attenuation = attenuation;
		InvalidateUniformBuffer();
		m_range = GetRange(m_color, m_intensity, m_attenuation);
	}
	
	void LightSourceEntity::SetFlickers(bool flickers)
	{
		m_flickers = flickers;
		InvalidateUniformBuffer();
	}
	
	ShadowMap* LightSourceEntity::GetShadowMap() const
	{
		return m_shadowMap.get();
	}
	
	void LightSourceEntity::SetShadowMode(EntityShadowModes mode)
	{
		if (mode == EntityShadowModes::None)
		{
			m_shadowMap = nullptr;
		}
		else
		{
			m_shadowMap = std::make_unique<ShadowMap>(mode == EntityShadowModes::Static);
			if (GetGameWorld() != nullptr)
				m_shadowMap->SetGameWorld(*GetGameWorld());
		}
		
		m_shadowMode = mode;
	}
	
	void LightSourceEntity::InvalidateShadowMap()
	{
		if (m_shadowMap != nullptr)
			m_shadowMap->FlagForRedraw();
	}
	
	void LightSourceEntity::OnSpawned(class GameWorld& gameWorld)
	{
		if (m_shadowMap != nullptr)
			m_shadowMap->SetGameWorld(gameWorld);
		Entity::OnSpawned(gameWorld);
	}
	
	nlohmann::json LightSourceEntity::Serialize() const
	{
		nlohmann::json json = Entity::Serialize();
		
		json["intensity"] = m_intensity;
		
		if (m_flickers)
			json["flickers"] = m_flickers;
		
		uint32_t hexColor = RgbColorToSRGBHex(m_color);
		char hexColorStr[7];
		sprintf(hexColorStr, "%06x", hexColor);
		
		json["color"] = hexColorStr;
		
		const char* shadowNames[] = { "none", "static", "dynamic" };
		json["shadows"] = shadowNames[static_cast<int>(m_shadowMode)];
		
		if (std::abs(m_attenuation.GetExponent() - 1.0f) > 1E-6)
			json["attenuation_exp"] = m_attenuation.GetExponent();
		if (m_attenuation.GetLinear() > 1E-6) //Can't be negative, so no need for abs
			json["attenuation_lin"] = m_attenuation.GetLinear();
		
		return json;
	}
	
	void LightSourceEntity::HandleEvent(const std::string& event, Entity* sender)
	{
		if (event == "EditorMoved")
			InvalidateShadowMap();
	}
	
	void LightSourceEntity::RenderLightSourceProperties()
	{
		const char* shadowModeStrings[] = { "None", "Static", "Dynamic" };
		
		int currentShadowMode = static_cast<int>(m_shadowMode);
		if (ImGui::Combo("Shadows", &currentShadowMode, shadowModeStrings, 3))
			SetShadowMode(static_cast<EntityShadowModes>(currentShadowMode));
		
		if (ImGui::InputFloat("Intensity", &m_intensity, 0.1f))
		{
			if (m_intensity < 0)
				m_intensity = 0;
			else
				m_uniformBufferOutOfDate = true;
		}
		
		float attenLinear = m_attenuation.GetLinear();
		float attenExp = m_attenuation.GetExponent();
		
		if (ImGui::InputFloat("Atten Linear", &attenLinear))
		{
			m_attenuation.SetLinear(glm::max(attenLinear, 0.0f));
			m_uniformBufferOutOfDate = true;
		}
		
		if (ImGui::InputFloat("Atten Exponent", &attenExp))
		{
			m_attenuation.SetExponent(glm::max(attenExp, 0.0f));
			m_uniformBufferOutOfDate = true;
		}
		
		if (ImGui::Checkbox("Flickers", &m_flickers))
			m_uniformBufferOutOfDate = true;
		
		glm::vec3 colorSrgb = glm::convertLinearToSRGB(m_color);
		if (ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&colorSrgb)))
		{
			m_color = glm::convertSRGBToLinear(colorSrgb);
			m_uniformBufferOutOfDate = true;
		}
	}
}
