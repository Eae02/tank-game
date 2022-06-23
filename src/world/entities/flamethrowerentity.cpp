#include "flamethrowerentity.h"
#include "hittable.h"
#include "../props/propsmanager.h"
#include "../gameworld.h"
#include "../particles/systems/flamethrower/flamethrowerparticlesystem.h"
#include "../../graphics/gl/shadermodule.h"
#include "../../graphics/quadmesh.h"
#include "../../graphics/spriterenderlist.h"
#include "../../updateinfo.h"
#include "../../utils/mathutils.h"
#include "../../utils/ioutils.h"
#include "../../audio/soundsmanager.h"

#include <imgui.h>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

namespace TankGame
{
	Lua::RegistryReference FlameThrowerEntity::s_metaTableRef;
	
	std::unique_ptr<FlameThrowerEntity::DistortionShader> FlameThrowerEntity::s_distortionShader;
	
	constexpr float FlameThrowerEntity::Size;
	
	static const float MaxLightIntensity = 10.0f;
	
	static const glm::vec3 LightColor = ParseColorHexCodeSRGB(0xff6c33);
	
	FlameThrowerEntity::FlameThrowerEntity(float flameLength, float enabledTime, float disabledTime, float startTime)
	    : m_audioSource(AudioSource::VolumeModes::Effect),
	      m_flameLength(flameLength), m_enabledTime(enabledTime), m_disabledTime(disabledTime), m_startTime(startTime),
	      m_flameThrowerPropClass(*PropsManager::GetInstance().GetPropClassByName("FlameThrower"))
	{
		float cannonAR = m_flameThrowerPropClass.GetTextureWidth() / static_cast<float>(m_flameThrowerPropClass.GetTextureHeight());
		
		GetTransform().SetScale({ Size, Size / cannonAR });
		
		m_audioSource.SetBuffer(SoundsManager::GetInstance().GetSound("FlameThrower"));
		m_audioSource.SetIsLooping(true);
		m_audioSource.SetAttenuationSettings(1.0f, 1.0f);
	}
	
	void FlameThrowerEntity::Update(const UpdateInfo& updateInfo)
	{
		bool enabled = m_manuallyEnabled;
		
		if (!m_manualEnableControl)
		{
			const bool alwaysOn = m_disabledTime < 1E-6f;
			
			const float cycleLength = m_enabledTime + m_disabledTime;
			const float time = std::fmod(updateInfo.m_gameTime - m_startTime + cycleLength, cycleLength);
			
			enabled = time < m_enabledTime || alwaysOn;
		}
		
		if (m_wasEnabled != enabled)
		{
			m_timeSinceEnableSwitch = 0.0f;
			m_wasEnabled = enabled;
		}
		
		float oldEnableProgress = m_enableProgress;
		
		const float transitionTime = 0.3f;
		float transitionDelta = (enabled ? 1 : -1) * (updateInfo.m_dt / transitionTime);
		m_enableProgress = glm::clamp(m_enableProgress + transitionDelta, 0.0f, 1.0f);
		
		// ** Updates whether the particle system is enabled **
		
		const float distToFocusSq = LengthSquared(GetGameWorld()->GetFocusLocation() - GetTransform().GetPosition());
		const float maxDistance = 40;
		
		const bool enablePS = enabled && distToFocusSq < (maxDistance * maxDistance);
		if (enablePS != m_psEnabled)
		{
			if (auto* psEntity = dynamic_cast<ParticleSystemEntityBase*>(m_particleSystemEntity.Get()))
				psEntity->SetEnabled(enablePS);
			m_psEnabled = enablePS;
		}
		
		// ** Updates light intensity **
		
		if (std::abs(oldEnableProgress - m_enableProgress) > 0.001f)
		{
			if (LightSourceEntity* lightEntity = dynamic_cast<LightSourceEntity*>(m_lightEntity.Get()))
			{
				lightEntity->SetIntensity(m_enableProgress * MaxLightIntensity);
			}
		}
		
		// ** Updates sound volume **
		
		if (GetGameWorld()->GetWorldType() == GameWorld::Types::Game)
		{
			const float maxVolume = 2.5f;
			m_audioSource.SetVolume(m_enableProgress * maxVolume);
			m_audioSource.SetPosition(GetMidFlamePos());
		}
		
		// ** Deals damage to entities in the fire **
		
		//Computes the distance the fire has traveled since the last enable switch.
		const float fireDist = glm::min(DirectionalFlameEmitter::FireSpeed * m_timeSinceEnableSwitch, m_flameLength);
		
		const float flameBeginPos = enabled ? 0 : fireDist;
		const float flameEndPos = enabled ? fireDist : m_flameLength;
		const float flameLength = flameEndPos - flameBeginPos;
		
		if (flameLength > 0.01f)
		{
			//The distance from the center of the emitter to the center of the flame.
			const float centerFlameDist = Size + (flameBeginPos + flameEndPos) * 0.5f;
			
			const OrientedRectangle hitRectangle(GetTransform().GetPosition() + GetTransform().GetForward() * centerFlameDist,
			                                     glm::vec2(0.4f, flameLength / 2.0f), GetTransform().GetRotation());
			
			GetGameWorld()->IterateIntersectingEntities(hitRectangle.GetOutsideRectangle(), [&] (Entity& entity)
			{
				if (Hittable* hittable = entity.AsHittable())
				{
					if (hitRectangle.GetIntersectInfo(hittable->GetHitCircle()).m_intersects)
						hittable->SetHp(hittable->GetHp() - DamagePerSecond * updateInfo.m_dt);
				}
			});
		}
		
		m_timeSinceEnableSwitch += updateInfo.m_dt;
	}
	
	void FlameThrowerEntity::Draw(SpriteRenderList& spriteRenderList) const
	{
		spriteRenderList.Add(GetTransform(), m_flameThrowerPropClass.GetMaterial(), 0.35f);
	}
	
	void FlameThrowerEntity::DrawDistortions() const
	{
		if (GetGameWorld()->GetWorldType() == GameWorld::Types::Editor || m_enableProgress < 1E-6f)
			return;
		
		//Loads the distortion shader if it hasn't been loaded yet
		if (s_distortionShader == nullptr)
		{
			auto vs = ShaderModule::FromFile(resDirectoryPath / "shaders" / "heat-dist.vs.glsl", GL_VERTEX_SHADER);
			auto fs = ShaderModule::FromFile(resDirectoryPath / "shaders" / "heat-dist.fs.glsl", GL_FRAGMENT_SHADER);
			
			ShaderProgram program(vs, fs);
			program.SetUniformBlockBinding("TextureMatricesUB", 1);
			program.SetTextureBinding("dudvMap", 0);
			
			int intensityLocation = program.GetUniformLocation("intensity");
			int transformLocation = program.GetUniformLocation("transform");
			
			Texture2D dudvMap = Texture2D::FromFile(resDirectoryPath / "heat-distortion.png", 2);
			dudvMap.SetWrapMode(GL_REPEAT);
			
			std::array<glm::mat2, 3> textureMatrices;
			
			//Generates texture matrices
			const float scale = 1.0f / 1.5f;
			for (size_t i = 0; i < textureMatrices.size(); i++)
			{
				float rotation = (i * glm::two_pi<float>()) / static_cast<float>(textureMatrices.size());
				float cosR = std::cos(rotation);
				float sinR = std::sin(rotation);
				
				textureMatrices[i] = glm::mat2(cosR * scale, -sinR, sinR, cosR * scale);
			}
			
			Buffer texureMatricesBuffer(textureMatrices.size() * sizeof(float) * 4, textureMatrices.data(), BufferUsage::StaticUBO);
			
			s_distortionShader.reset(new DistortionShader { std::move(program), transformLocation, intensityLocation,
			                                                std::move(dudvMap), std::move(texureMatricesBuffer) });
			CallOnClose([] { s_distortionShader = nullptr; });
		}
		
		s_distortionShader->m_shader.Use();
		
		s_distortionShader->m_dudvMap.Bind(0);
		
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, s_distortionShader->m_textureMatricesBuffer.GetID());
		
		const float expansionFactor = 1.5f;
		
		//Sets up the distortion quad transform
		Transform distortionQuadTransform;
		distortionQuadTransform.SetPosition(GetTransform().GetPosition() + GetTransform().GetForward() * (Size + m_flameLength / 2.0f));
		distortionQuadTransform.SetScale({ 0.5f * expansionFactor, m_flameLength * expansionFactor * 0.5f });
		distortionQuadTransform.SetRotation(GetTransform().GetRotation());
		
		//Sets shader parameters
		glProgramUniformMatrix3fv(s_distortionShader->m_shader.GetID(), s_distortionShader->m_transformLocation, 1,
		                          GL_FALSE, reinterpret_cast<const GLfloat*>(&distortionQuadTransform.GetMatrix()));
		
		glProgramUniform1f(s_distortionShader->m_shader.GetID(), s_distortionShader->m_intensityLocation,
		                   m_enableProgress * 40.0f);
		
		//Draws the quad
		QuadMesh::GetInstance().BindVAO();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	void FlameThrowerEntity::DrawEditorUI(UIRenderer& uiRenderer, const glm::mat3& uiViewMatrix) const
	{
		glm::vec2 nearPos = GetTransform().GetPosition() + GetTransform().GetForward() * Size;
		glm::vec2 farPos = nearPos + GetTransform().GetForward() * m_flameLength;
		
		glm::vec2 left(-GetTransform().GetForward().y, GetTransform().GetForward().x);
		
		std::array<glm::vec2, 4> flameAreaCorners = 
		{
			nearPos + left * 0.1f,
			nearPos - left * 0.1f,
			farPos + left * 0.5f,
			farPos - left * 0.5f
		};
		
		for (glm::vec2& corner : flameAreaCorners)
			corner = glm::vec2(uiViewMatrix * glm::vec3(corner, 1.0f));
		
		uiRenderer.DrawQuad(flameAreaCorners, glm::vec4(LightColor, 0.8f));
	}
	
	void FlameThrowerEntity::OnSpawned(GameWorld& gameWorld)
	{
		Entity::OnSpawned(gameWorld);
		
		if (gameWorld.GetWorldType() == GameWorld::Types::Game)
		{
			m_audioSource.SetVolume(0.0f);
			m_audioSource.Play();
		}
		
		SpawnChildEntities();
	}
	
	void FlameThrowerEntity::OnDespawning()
	{
		m_particleSystemEntity.Despawn();
		m_lightEntity.Despawn();
	}
	
	void FlameThrowerEntity::EditorMoved()
	{
		TransformModified();
	}
	
	void FlameThrowerEntity::RenderProperties()
	{
		RenderBaseProperties(Transform::Properties::Position | Transform::Properties::Rotation);
		
		if (ImGui::InputFloat("Flame Length", &m_flameLength, 0.5f, 1.0f))
		{
			m_flameLength = std::max(m_flameLength, 1.0f);
			SpawnChildEntities();
		}
		
		ImGui::Checkbox("Scripted Enable / Disable", &m_manualEnableControl);
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("If enabled, the flamethrower is controlled by scripts instead of automatically based on time.");
		
		if (m_manualEnableControl)
		{
			ImGui::Checkbox("Enabled", &m_manuallyEnabled);
		}
		else
		{
			if (ImGui::InputFloat("Enabled Time", &m_enabledTime, 0.1f, 1.0f))
				m_enabledTime = std::max(m_enabledTime, 1.0f);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("The number of seconds which the flamethrower will be turned on.");
			
			if (ImGui::InputFloat("Disabled Time", &m_disabledTime, 0.1f, 1.0f))
				m_disabledTime = std::max(m_disabledTime, 0.0f);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("The number of seconds which the flamethrower will be turned off.");
			
			ImGui::SliderFloat("Start Time", &m_startTime, 0.0f, m_enabledTime + m_disabledTime);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Offset (in seconds) for the start of each cycle.");
			
			m_startTime = glm::clamp(m_startTime, 0.0f, m_enabledTime + m_disabledTime);
		}
	}
	
	const char* FlameThrowerEntity::GetObjectName() const
	{
		return "Flame Thrower";
	}
	
	const char* FlameThrowerEntity::GetSerializeClassName() const
	{
		return "FlameThrower";
	}
	
	nlohmann::json FlameThrowerEntity::Serialize() const
	{
		nlohmann::json json;
		
		json["transform"] = GetTransform().Serialize(Transform::Properties::Position | Transform::Properties::Rotation);
		
		json["flame_length"] = m_flameLength;
		
		if (m_manualEnableControl)
		{
			json["manual_control"] = true;
			json["enabled"] = m_manuallyEnabled;
		}
		else
		{
			json["enabled_time"] = m_enabledTime;
			json["disabled_time"] = m_disabledTime;
			json["start_time"] = m_startTime;
		}
		
		return json;
	}
	
	std::unique_ptr<Entity> FlameThrowerEntity::Clone() const
	{
		auto clone = std::make_unique<FlameThrowerEntity>(m_flameLength, m_enabledTime, m_disabledTime, m_startTime);
		clone->GetTransform() = GetTransform();
		return clone;
	}
	
	void FlameThrowerEntity::PushLuaMetaTable(lua_State* state) const
	{
		if (!s_metaTableRef)
		{
			NewLuaMetaTable(state);
			
			Entity::PushLuaMetaTable(state);
			lua_setmetatable(state, -2);
			
			// ** setEnabled **
			lua_pushcfunction(state, [] (lua_State* state) -> int
			{
				static_cast<FlameThrowerEntity*>(LuaGetInstance(state))->SetEnabled(lua_toboolean(state, 2));
				return 0;
			});
			lua_setfield(state, -2, "setEnabled");
			
			s_metaTableRef = Lua::RegistryReference::PopAndCreate(state);
			CallOnClose([] { s_metaTableRef = { }; });
		}
		
		s_metaTableRef.Load(state);
	}
	
	void FlameThrowerEntity::SpawnChildEntities()
	{
		if (GetGameWorld()->GetWorldType() == GameWorld::Types::Editor)
			return;
		
		m_particleSystemEntity.Despawn();
		m_lightEntity.Despawn();
		
		auto psEntity = MakeParticleSystemEntity<FlameThrowerParticleSystem>({ m_flameLength, GetGameWorld()->GetParticlesManager() });
		psEntity->SetEditorVisible(false);
		
		auto lightEntity = std::make_unique<PointLightEntity>(LightColor, 0, Attenuation(0, 0.5f ));
		lightEntity->SetEditorVisible(false);
		lightEntity->SetShadowMode(EntityShadowModes::Static);
		
		SetChildEntitiesTransform(psEntity.get(), lightEntity.get());
		
		m_particleSystemEntity = GetGameWorld()->Spawn(std::move(psEntity));
		m_lightEntity = GetGameWorld()->Spawn(std::move(lightEntity));
	}
	
	void FlameThrowerEntity::SetChildEntitiesTransform(Entity* psEntity, Entity* lightEntity)
	{
		if (psEntity != nullptr)
		{
			psEntity->GetTransform().SetPosition(GetTransform().GetPosition() + GetTransform().GetForward() * Size);
			psEntity->GetTransform().SetRotation(GetTransform().GetRotation());
			
			if (lightEntity != nullptr)
			{
				lightEntity->GetTransform().SetPosition(GetMidFlamePos());
			}
		}
	}
	
	glm::vec2 FlameThrowerEntity::GetMidFlamePos() const
	{
		return GetTransform().GetPosition() + GetTransform().GetForward() * (Size + m_flameLength / 2.0f);
	}
	
	Circle FlameThrowerEntity::GetBoundingCircle() const
	{
		return Circle(GetTransform().GetPosition(), Size);
	}
	
	ColliderInfo FlameThrowerEntity::GetColliderInfo() const
	{
		return OrientedRectangle::FromTransformedNDC(GetTransform());
	}
	
	bool FlameThrowerEntity::IsStaticCollider() const
	{
		return true;
	}
	
	CollidableTypes FlameThrowerEntity::GetCollidableType() const
	{
		return CollidableTypes::Object;
	}
}
