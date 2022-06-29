#pragma once

#include "particleinfo.h"

namespace TankGame
{
	class Particle : private ParticleInfo
	{
	public:
		Particle() = default;
		
		explicit Particle(const ParticleInfo& info);
		
		inline const glm::vec2& GetPosition() const
		{ return m_position; }
		inline const glm::vec2& GetVelocity() const
		{ return m_velocity; }
		inline float GetRotation() const
		{ return m_rotation; }
		inline float GetRotationVelocity() const
		{ return m_rotationVelocity; }
		
		inline int GetTextureLayer() const
		{ return m_textureLayer; }
		
		inline double GetSpawnTime() const
		{ return m_spawnTime; }
		inline double GetLifeTime() const
		{ return m_lifeTime; }
		inline double GetDeathTime() const
		{ return m_spawnTime + m_lifeTime; }
		
		inline float GetSize(float timeInterpolation) const
		{ return glm::mix(m_beginSize, m_endSize, timeInterpolation); }
		inline float GetOpacity(float timeInterpolation) const
		{ return glm::mix(m_beginOpacity, m_endOpacity, timeInterpolation); }
		
		inline float GetTimeInterpolation(double time) const
		{ return static_cast<float>((time - m_spawnTime) / m_lifeTime); }
		
		glm::mat3 GetWorldMatrix(float timeInterpolation) const;
		
		inline float GetBoundingSize() const { return m_boundingSize; }
		
		void Update(float dt);
		
	private:
		double m_spawnTime;
		float m_boundingSize;
	};
}
