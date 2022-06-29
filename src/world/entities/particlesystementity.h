#pragma once

#include "../entity.h"
#include "../particles/systems/particlesystem.h"
#include "../../utils/abstract.h"


namespace TankGame
{
	class ParticleSystemEntityBase : public virtual Entity, public ILastFrameTransformProvider
	{
	public:
		inline IParticleSystem& GetParticleSystem() const
		{ return m_particleSystem; }
		
		virtual const Transform& GetLastFrameTransform() const final override
		{ return m_hasLastFrameTransform ? m_lastFrameTransform : GetTransform(); }
		
		void UpdateLastFrameTransform();
		
		inline double GetDeathTime() const
		{ return m_deathTime; }
		
		inline void SetEnabled(bool enabled)
		{ m_enabled = enabled; }
		inline bool IsEnabled() const
		{ return m_enabled; }
		
	protected:
		ParticleSystemEntityBase(IParticleSystem& particleSystem, double lifeTime);
		
	private:
		IParticleSystem& m_particleSystem;
		
		bool m_hasLastFrameTransform = false;
		Transform m_lastFrameTransform;
		
		double m_deathTime;
		
		bool m_enabled = true;
	};
	
	template <typename ParticleSystemTp>
	class ParticleSystemEntity : public ParticleSystemEntityBase
	{
	public:
		explicit ParticleSystemEntity(ParticleSystemTp particleSystem,
		                              double lifeTime = std::numeric_limits<double>::infinity())
			: ParticleSystemEntityBase(m_particleSystem, lifeTime), m_particleSystem(std::move(particleSystem)) { }
		
		virtual void OnSpawned(class GameWorld& gameWorld) override
		{
			m_particleSystem.SetTransformationProvider(this);
			Entity::OnSpawned(gameWorld);
		}
		
	private:
		ParticleSystemTp m_particleSystem;
	};
	
	template <typename ParticleSystemTp>
	inline auto MakeParticleSystemEntity(ParticleSystemTp particleSystem,
	                                     double lifeTime = std::numeric_limits<double>::infinity())
	{
		return std::make_unique<ParticleSystemEntity<ParticleSystemTp>>(std::move(particleSystem), lifeTime);
	}
}
