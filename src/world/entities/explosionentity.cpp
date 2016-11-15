#include "explosionentity.h"
#include "../gameworld.h"
#include "../../updateinfo.h"
#include "../../utils/utils.h"
#include "../../utils/ioutils.h"
#include "../../audio/soundeffectplayer.h"
#include "../../graphics/gl/shadermodule.h"
#include "../../graphics/quadmesh.h"

#include <GLFW/glfw3.h>
#include <initializer_list>

namespace TankGame
{
	static SoundEffectPlayer explosionEffectPlayer("Explosion");
	
	StackObject<ShaderProgram> ExplosionEntity::s_distortionShader;
	
	static const float LIFE_TIME = 0.4f;
	static const float LIGHT_INTENSITY = 40.0f;
	
	static const float BAST_END_RADIUS = 5;
	
	void ExplosionEntity::BindShader()
	{
		if (s_distortionShader.IsNull())
		{
			auto vs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "blastwave.vs.glsl", GL_VERTEX_SHADER);
			auto fs = ShaderModule::FromFile(GetResDirectory() / "shaders" / "blastwave.fs.glsl", GL_FRAGMENT_SHADER);
			
			s_distortionShader.Construct<std::initializer_list<const ShaderModule*>>({ &vs, &fs });
			
			CallOnClose([] { s_distortionShader.Destroy(); });
		}
		
		s_distortionShader->Use();
	}
	
	ExplosionEntity::ExplosionEntity(ParticlesManager& particlesManager)
	    : PointLightEntity(ParseColorHexCodeSRGB(0xff6c33), LIGHT_INTENSITY, Attenuation(0, 0.5f)),
	      ParticleSystemEntity(ExplosionParticleSystem(particlesManager), LIFE_TIME),
	      m_blastSettingsUniformBuffer(BufferAllocator::GetInstance().AllocateUnique(sizeof(float) * 4, GL_MAP_WRITE_BIT))
	{
		SetShadowMode(EntityShadowModes::Dynamic);
		UpdateBlastSettings(0);
	}
	
	Circle ExplosionEntity::GetBoundingCircle() const
	{
		Circle circle = PointLightEntity::GetBoundingCircle();
		if (circle.GetRadius() < BAST_END_RADIUS)
			circle.SetRadius(BAST_END_RADIUS);
		return circle;
	}
	
	void ExplosionEntity::DrawDistortions() const
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, 1, *m_blastSettingsUniformBuffer);
		
		BindShader();
		
		QuadMesh::GetInstance().GetVAO().Bind();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	
	void ExplosionEntity::UpdateBlastSettings(float timeInterpol)
	{
		void* blastSettings = glMapNamedBufferRange(*m_blastSettingsUniformBuffer, 0, sizeof(float) * 4,
		                                            GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		
		reinterpret_cast<float*>(blastSettings)[0] = GetTransform().GetPosition().x;
		reinterpret_cast<float*>(blastSettings)[1] = GetTransform().GetPosition().y;
		reinterpret_cast<float*>(blastSettings)[2] = 4 * (1 - timeInterpol);
		reinterpret_cast<float*>(blastSettings)[3] = std::sqrt(timeInterpol) * BAST_END_RADIUS;
		
		glUnmapNamedBuffer(*m_blastSettingsUniformBuffer);
	}
	
	void ExplosionEntity::Update(const UpdateInfo& updateInfo)
	{
		double timeSinceSpawn = glfwGetTime() - (GetDeathTime() - LIFE_TIME);
		float timeInterpol = glm::clamp(timeSinceSpawn / LIFE_TIME, 0.0, 1.0);
		
		SetIntensity(LIGHT_INTENSITY * glm::min(2.0f - timeInterpol * 2.0f, 1.0f));
		
		UpdateBlastSettings(glm::clamp(timeSinceSpawn / (LIFE_TIME * 1.5f), 0.0, 1.0));
	}
	
	void ExplosionEntity::OnSpawned(GameWorld& gameWorld)
	{
		float distToFocus = glm::distance(gameWorld.GetFocusLocation(), GetTransform().GetPosition());
		gameWorld.ShakeCamera(0.25f, 2.0f / glm::max(distToFocus, 0.1f));
		
		explosionEffectPlayer.Play(GetTransform().GetPosition(), 10.0f, 1.0f);
		
		PointLightEntity::OnSpawned(gameWorld);
		ParticleSystemEntity::OnSpawned(gameWorld);
	}
}
