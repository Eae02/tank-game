#pragma once

#include "../particleemitter.h"
#include "../spherevec2generator.h"

#include <memory>

namespace TankGame
{
	class SparkEmitter : public ParticleEmitter
	{
	public:
		SparkEmitter(ParticlesManager& particlesManager, float maxAngleDivergence, float speedMul = 1);
		
		void SetTransformationProvider(const class ITransformationProvider* transformationProvider);
		
	protected:
		virtual glm::vec2 GeneratePosition(float subframeInterpolation) const override;
		virtual glm::vec2 GenerateVelocity(float subframeInterpolation) const override;
		
	private:
		static std::unique_ptr<Texture2DArray> s_sparkTextureArray;
		
		SphereVec2Generator m_positionGenerator;
		SphereVec2Generator m_velocityGenerator;
	};
}
