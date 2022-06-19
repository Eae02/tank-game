#include "lightsourceentity.h"
#include "ilightsource.h"
#include "../../utils/mathutils.h"
#include "../../utils/utils.h"

#include <glm/gtc/color_space.hpp>
#include <imgui.h>
#include <cstdio>
#include <random>

namespace TankGame
{
	Lua::RegistryReference LightSourceEntity::s_metaTableRef;
	
	LightSourceEntity::LightSourceEntity(glm::vec3 color, float intensity, Attenuation attenuation, float height)
	    : m_flickerOffset(GenerateFlickerOffset()),
	      m_uniformBuffer(BufferAllocator::GetInstance().AllocateUnique(sizeof(LightUniformBufferData), BufferUsage::DynamicData)),
	      m_color(color), m_intensity(intensity), m_attenuation(attenuation), m_height(height),
	      m_range(GetRange(color, intensity, attenuation)) { }
	
	Circle LightSourceEntity::GetBoundingCircle() const
	{
		return Circle(GetTransform().GetPosition(), m_range);
	}
	
	static constexpr float FLICKER_INTENSITY = 0.12f;
	
	void LightSourceEntity::Bind() const
	{
		if (m_uniformBufferOutOfDate)
		{
			LightUniformBufferData data = {};
			if (m_enabled)
			{
				data.colorTimesIntensity[0] = m_color.r * m_intensity;
				data.colorTimesIntensity[1] = m_color.g * m_intensity;
				data.colorTimesIntensity[2] = m_color.b * m_intensity;
			}
			data.extra = GetExtraUniformValue();
			data.attenLin = m_attenuation.GetLinear();
			data.attenExp = m_attenuation.GetExponent();
			data.flickerIntensity = m_flickers ? FLICKER_INTENSITY : 0.0f;
			data.flickerOffset = m_flickerOffset;
			
			m_uniformBuffer->Update(0, sizeof(LightUniformBufferData), &data);
			m_uniformBufferOutOfDate = false;
		}
		
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uniformBuffer->GetID());
		
		glm::vec2 pos2D = GetTransform().GetPosition();
		glUniform3f(GetPositionUniformLocation(), pos2D.x, m_height, pos2D.y);
		
		glm::mat3 worldTransform = MapNDCToRectangle(GetBoundingRectangle());
		glUniformMatrix3fv(GetWorldTransformUniformLocation(), 1, GL_FALSE, reinterpret_cast<GLfloat*>(&worldTransform));
		
		if (m_shadowMap != nullptr)
			m_shadowMap->Bind(SHADOW_MAP_TEXTURE_BINDING);
		else
			ShadowMap::BindDefault(SHADOW_MAP_TEXTURE_BINDING);
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
	
	void LightSourceEntity::SetEnabled(bool enabled)
	{
		m_enabled = enabled;
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
		
		json["enabled"] = m_enabled;
		
		const char* shadowNames[] = { "none", "static", "dynamic" };
		json["shadows"] = shadowNames[static_cast<int>(m_shadowMode)];
		
		if (std::abs(m_attenuation.GetExponent() - 1.0f) > 1E-6)
			json["attenuation_exp"] = m_attenuation.GetExponent();
		if (m_attenuation.GetLinear() > 1E-6) //Can't be negative, so no need for abs
			json["attenuation_lin"] = m_attenuation.GetLinear();
		
		return json;
	}
	
	void LightSourceEntity::EditorMoved()
	{
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
		
		if (ImGui::Checkbox("Enabled", &m_enabled))
			m_uniformBufferOutOfDate = true;
		
		if (ImGui::Checkbox("Flickers", &m_flickers))
			m_uniformBufferOutOfDate = true;
		
		glm::vec3 colorSrgb = glm::convertLinearToSRGB(m_color);
		if (ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&colorSrgb)))
		{
			m_color = glm::convertSRGBToLinear(colorSrgb);
			m_uniformBufferOutOfDate = true;
		}
	}
	
	void LightSourceEntity::PushLuaMetaTable(lua_State* state) const
	{
		if (!s_metaTableRef)
		{
			NewLuaMetaTable(state);
			
			Entity::PushLuaMetaTable(state);
			lua_setmetatable(state, -2);
			
			// ** setEnabled **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				dynamic_cast<LightSourceEntity*>(LuaGetInstance(state))->SetEnabled(lua_toboolean(state, 2));
				return 0;
			});
			lua_setfield(state, -2, "setEnabled");
			
			// ** enabled **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				lua_pushboolean(state, dynamic_cast<const LightSourceEntity*>(LuaGetInstance(state))->Enabled());
				return 1;
			});
			lua_setfield(state, -2, "enabled");
			
			s_metaTableRef = Lua::RegistryReference::PopAndCreate(state);
			CallOnClose([] { s_metaTableRef = { }; });
		}
		
		s_metaTableRef.Load(state);
	}
}
