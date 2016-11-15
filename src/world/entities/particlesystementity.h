#pragma once

#include "../entity.h"
#include "../particles/systems/particlesystem.h"
#include "../../utils/memory/stackobject.h"
#include "../../utils/abstract.h"

#include <limits>

namespace TankGame
{
	class ParticleSystemEntityBase : public virtual Entity, public ILastFrameTransformProvider
	{
	public:
		inline IParticleSystem& GetParticleSystem() const
		{ return m_particleSystem; }
		
		virtual const Transform& GetLastFrameTransform() const final override
		{ return m_lastFrameTransform.IsNull() ? GetTransform() : *m_lastFrameTransform; }
		
		inline void UpdateLastFrameTransform()
		{ m_lastFrameTransform.Construct(GetTransform()); }
		
		inline double GetDeathTime() const
		{ return m_deathTime; }
		
	protected:
		ParticleSystemEntityBase(IParticleSystem& particleSystem, double lifeTime);
		
	private:
		IParticleSystem& m_particleSystem;
		StackObject<Transform> m_lastFrameTransform;
		
		double m_deathTime;
	};
	
	template <typename T>
	class ParticleSystemEntity : public ParticleSystemEntityBase
	{
	public:
		explicit ParticleSystemEntity(T&& particleSystem, double lifeTime = std::numeric_limits<double>::infinity())
			: ParticleSystemEntityBase(m_particleSystem, lifeTime), m_particleSystem(std::move(particleSystem)) { }
		
		virtual void OnSpawned(class GameWorld& gameWorld) override
		{
			m_particleSystem.SetTransformationProvider(this);
			Entity::OnSpawned(gameWorld);
		}
		
	private:
		T m_particleSystem;
	};
}
