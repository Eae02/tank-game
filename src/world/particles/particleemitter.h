#pragma once

#include "particle.h"
#include "particlepool.h"
#include "particlesmanager.h"
#include "ivec2generator.h"
#include "../../graphics/gl/texture2darray.h"
#include "../../graphics/gl/bufferallocator.h"

#include <random>

namespace TankGame
{
	class ParticleEmitter : public Abstract
	{
	public:
		ParticleEmitter(ParticlesManager& particlesManager);
		
		inline float GetSpawnRate() const
		{ return m_emissionRate; }
		inline void SetSpawnRate(float spawnRate)
		{ m_emissionRate = spawnRate; }
		
		inline void SetBeginOpacity(float opacity)
		{ SetBeginOpacity(opacity, opacity); }
		inline void SetBeginOpacity(float min, float max)
		{ m_beginOpacityDist = std::uniform_real_distribution<float>(min, max); }
		
		inline void SetEndOpacity(float opacity)
		{ SetEndOpacity(opacity, opacity); }
		inline void SetEndOpacity(float min, float max)
		{ m_endOpacityDist = std::uniform_real_distribution<float>(min, max); }
		
		inline void SetBeginSize(float size)
		{ SetBeginSize(size, size); }
		inline void SetBeginSize(float min, float max)
		{ m_beginSizeDist = std::uniform_real_distribution<float>(min, max); }
		
		inline void SetEndSize(float size)
		{ SetEndSize(size, size); }
		inline void SetEndSize(float min, float max)
		{ m_endSizeDist = std::uniform_real_distribution<float>(min, max); }
		
		inline void SetRotation(float min, float max)
		{ m_rotationDist = std::uniform_real_distribution<float>(min, max); }
		inline void SetRotationSpeed(float min, float max)
		{ m_rotationSpeedDist = std::uniform_real_distribution<float>(min, max); }
		
		inline void SetLifeTime(double min, double max)
		{ m_lifeTimeDist = std::uniform_real_distribution<double>(min, max); }
		
		inline void SetAlignParticlesToVelocity(bool alignToVelocity)
		{ m_alignParticlesToVelocity = alignToVelocity; }
		
		void SetUseAdditiveBlending(bool useAdditiveBlending);
		inline bool UseAdditiveBlending() const
		{ return m_useAdditiveBlending; }
		
		bool HasParticles() const;
		
		void SetTextureArray(const Texture2DArray& textureArray);
		
		void SpawnParticles(float elapsedTime);
		
		void Render(class ParticleRenderer& renderer, const struct ViewInfo& viewInfo) const;
		
	protected:
		virtual glm::vec2 GeneratePosition(float subframeInterpolation) const = 0;
		virtual glm::vec2 GenerateVelocity(float subframeInterpolation) const { return glm::vec2(0.0f); }
		
		static inline IVec2Generator::RNG& GetRandom()
		{ return s_random; } 
		
	private:
		static IVec2Generator::RNG s_random;
		
		ParticleInfo GetParticleInfo(float subframeInterpolation);
		
		std::vector<ParticlePoolHandle> m_particlePools;
		
		bool m_useAdditiveBlending = false;
		
		bool m_alignParticlesToVelocity = false;
		
		ParticlesManager& m_particlesManager;
		
		const Texture2DArray* m_textureArray = nullptr;
		std::uniform_int_distribution<int> m_layerDist;
		
		float m_timeNotEmitted = 0.0f; //Accumulates the amount of time which hasn't contributed to an emitted particle.
		
		float m_emissionRate = 10; //The number of particles to spawn per second.
		
		float m_textureAspectRatio = 0;
		
		std::uniform_real_distribution<double> m_lifeTimeDist;
		std::uniform_real_distribution<float> m_rotationDist;
		std::uniform_real_distribution<float> m_rotationSpeedDist;
		
		std::uniform_real_distribution<float> m_beginOpacityDist;
		std::uniform_real_distribution<float> m_endOpacityDist;
		
		std::uniform_real_distribution<float> m_beginSizeDist;
		std::uniform_real_distribution<float> m_endSizeDist;
	};
}
