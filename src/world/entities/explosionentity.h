#pragma once

#include "particlesystementity.h"
#include "../particles/systems/explosionparticlesystem.h"
#include "../lights/pointlightentity.h"
#include "../../audio/audiosource.h"

namespace TankGame
{
	class ExplosionEntity : public PointLightEntity, public ParticleSystemEntity<ExplosionParticleSystem>,
	        public Entity::IUpdateable, public Entity::IDistortionDrawable
	{
	public:
		explicit ExplosionEntity(ParticlesManager& particlesManager);
		
		virtual Circle GetBoundingCircle() const override;
		
		virtual void DrawDistortions() const override;
		
		virtual void Update(const class UpdateInfo& updateInfo) override;
		
		virtual void OnSpawned(class GameWorld& gameWorld) override;
		
		virtual Entity::IUpdateable* AsUpdatable() final override
		{ return this; }
		virtual const Entity::IDistortionDrawable* AsDistortionDrawable() const final override
		{ return this; }
		
		virtual const char* GetSerializeClassName() const override
		{ return nullptr; }
		
	private:
		static void BindShader();
		
		void UpdateBlastSettings(float timeInterpol);
		
		static std::unique_ptr<ShaderProgram> s_distortionShader;
		
		AudioSource m_audioSource;
		
		float m_blastRadiusUniformValue;
		float m_blastIntensityUniformValue;
	};
}
